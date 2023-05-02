#include "../csapp.h"

void doit(int fd);
void read_requesthdrs(rio_t *rp);
int parse_uri(char *uri,char *filename,char *cgiargs);
void clienterror(int fd,char *cause,char *errnum,char *shortmsg,char *longmsg);
void serve_static(int fd,char *filename,int filesize);
void serve_dynamic(int fd,char *filename,char *cgiargs);
void get_filetype(char *filename,char *filetype);


int main(int argc,char **argv)
{
    int listenfd,connfd;
    char hostname[MAXLINE],port[MAXLINE];
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;

    if(argc != 2)
    {
        fprintf(stderr,"usage: %s <port>\n",argv[0]);
        exit(1);
    }
    listenfd = Open_listenfd(argv[1]);
    while(1)
    {
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd,(SA*)&clientaddr,&clientlen);
        getnameinfo((SA*)&clientaddr,clientlen,hostname,MAXLINE,port,MAXLINE,0);
        printf("Accept connnection from (%s,%s)\n",hostname,port);
        doit(connfd);
        Close(connfd);
        printf("goodbye to (%s,%s)\n",hostname,port);
    }
}
void doit(int fd)
{
    int is_static;
    struct stat sbuf;
    char buf[MAXLINE],method[MAXLINE],uri[MAXLINE],version[MAXLINE];
    char filename[MAXLINE],cgiargs[MAXLINE];
    rio_t rio;
    Rio_readinitb(&rio,fd);
    Rio_readlineb(&rio,buf,MAXLINE);
    printf("Request headers:\n");
    printf("%s",buf);
    sscanf(buf,"%s %s %s",method,uri,version);
    if(strcasecmp(method,"GET")) // 不是get方法
    {
        clienterror(fd,method,"501","Not implemented","Tiny doesn't implemented this method");
        return;
    }
    read_requesthdrs(&rio);//读取其他headers信息并且忽略
    is_static = parse_uri(uri,filename,cgiargs);// 解析uri后判断是否为动态请求
    // 将filename的文件属性信息保存到sbuf
    if(stat(filename,&sbuf) < 0)
    {
        clienterror(fd,filename,"404","Not found","Tiny couldn't find this file");
        return;
    }
    // is_static == 1 表示是静态文件
    if(is_static)
    {
        // 文件不是普通文件或者文件不可读
        if(!(S_ISREG(sbuf.st_mode))||!(S_IRUSR&sbuf.st_mode))
        {
            clienterror(fd,filename,"403","Forbidden","Tiny couldn't read the file");
            return;
        }
        serve_static(fd,filename,sbuf.st_size);
    }else{
        // 文件不是普通文件或者文件不可执行
        if(!(S_ISREG(sbuf.st_mode))||!(S_IXUSR&sbuf.st_mode))
        {
            printf("%s-%s\n",filename,cgiargs);
            clienterror(fd,filename,"403","Forbidden","Tiny couldn't run the CGI file");
            return;
        }
        serve_dynamic(fd,filename,cgiargs);
    }

}
//不会使用请求报文头部的第一行以外的其他信息，用该函数读取并且忽略
void read_requesthdrs(rio_t *rp)
{
    char buf[MAXLINE];
    Rio_readlineb(rp,buf,MAXLINE);
    while(strcmp(buf,"\r\n"))
    {
        Rio_readlineb(rp,buf,MAXLINE);
        printf("%s",buf);
    }
    return;
}

// 把uri字符串解析成文件名和cgi参数
int parse_uri(char *uri,char *filename,char *cgiargs)
{
    char *ptr;
    if(!strstr(uri,"cgi-bin"))//uri不包含动态文件，因此为静态
    {
        strcpy(cgiargs,"");//给cgiargs赋值为空，因为请求静态文件，不需要参数
        strcpy(filename,".");
        strcat(filename,uri);//拼接字符串
        if(uri[strlen(uri)-1] == '/') // 如果相对路径以/结尾
        {
            strcat(filename,"html/home.html");//拼接默认对主页html文件
        }
        return 1;
    }else{
        ptr = strchr(uri,'?');// 返回？在uri中第一个出现的位置
        if(ptr)
        {
            strcpy(cgiargs,ptr+1);
            *ptr = '\0';
        }else{
            strcpy(cgiargs,"");//如果没有找到？,则没有参数，置空
        }
        strcpy(filename,".");
        strcat(filename,uri);
        return 0;
    }
}

void clienterror(int fd,char *cause,char *errnum,char *shortmsg,char *longmsg)
{
    char buf[MAXLINE],body[MAXBUF];

    sprintf(buf,"HTTP/1.0 %s %s\r\n",errnum,shortmsg);
    Rio_writen(fd,buf,strlen(buf));

    sprintf(buf,"Content-type: text/html\r\n");
    Rio_writen(fd,buf,strlen(buf));

    sprintf(body,"<html>\r\n<title>Tiny Error</title>");
    sprintf(body,"%s<body bgcolor=""ffffff"">\r\n",body);
    sprintf(body,"%s %s: %s\r\n",body,errnum,shortmsg);
    sprintf(body,"%s<p>%s:%s</p>\r\n",body,longmsg,cause);
    sprintf(body,"%s</hr><em>The Tiny webserver</em>\r\n",body);
    sprintf(body,"%s</body>\r\n</html>\r\n",body);

    sprintf(buf,"Content-length:%d\r\n\r\n",(int)strlen(body));
    Rio_writen(fd,buf,strlen(buf));
    Rio_writen(fd,body,strlen(body));
    return;
}

void serve_static(int fd,char *filename,int filesize)
{
    int srcfd;
    char *srcp,filetype[MAXLINE],buf[MAXBUF];
    get_filetype(filename,filetype);
    sprintf(buf,"HTTP/1.0 200 OK\r\n");
    sprintf(buf,"%sServer: Tiny Web Server\r\n",buf);
    sprintf(buf,"%sConnection: close\r\n",buf);
    sprintf(buf,"%sContent-length: %d\r\n",buf,filesize);
    sprintf(buf,"%sContent-type: %s\r\n\r\n",buf,filetype);
    Rio_writen(fd,buf,strlen(buf));

    printf("Response headers:\n");
    printf("%s",buf);

    srcfd = Open(filename,O_RDONLY,0);
    // 将文件以页为单位映射到内存的虚拟地址空间
    // 0 设置为0时表示由系统决定映射区的起始地址
    // filesize 以字节为单位，不足一内存页按一内存页处理
    // PROT_READ 页内容可以被读取
    // MAP_PRIVATE //建立一个写入时拷贝的私有映射。内存区域的写入不会影响到原文件
    // srcfd有效的文件描述词
    // mmap()系统调用使得进程之间通过映射同一个普通文件实现共享内存。
    // 普通文件被映射到进程地址空间后，进程可以像访问普通内存一样对文件进行访问，
    srcp = Mmap(0,filesize,PROT_READ,MAP_PRIVATE,srcfd,0);
    Close(srcfd);
    Rio_writen(fd,srcp,filesize);
    // 该调用在进程地址空间中解除一个映射关系,addr是调用mmap()时返回的地址，len是映射区的大小
    Munmap(srcp,filesize);// 避免潜在的内存泄露
    return;
}
void serve_dynamic(int fd,char *filename,char *cgiargs)
{
    char buf[MAXLINE],*emptylist[] = {NULL};
    sprintf(buf,"HTTP/1.0 200 OK\r\n");
    Rio_writen(fd,buf,strlen(buf));
    sprintf(buf,"Server: Tiny Web Server\r\n");
    Rio_writen(fd,buf,strlen(buf));
    // 创建子进程
    if(Fork() == 0)
    {
        setenv("QUERY_STRING",cgiargs,1);//设置环境变量QUERY_STRING的值为cgiargs，1表示可重写
        //将子进程的标准输出重定向到fd描述符
        //这个的fd为客户端连接套接字文件描述符
        Dup2(fd,STDOUT_FILENO);
        // 指向filename指向的动态文件
        // emptylist是要调用的程序执行的参数序列，也就是我们要调用的程序需要传入的参数
        // environ 同样也是参数序列，一般来说他是一种键值对的形式 key=value. 作为我们是新程序的环境
        Execve(filename,emptylist,environ);
    }
    //父进程等待子进程结束
    Wait(NULL);
}
void get_filetype(char *filename,char *filetype)
{
    if(strstr(filename,".html"))
        strcpy(filetype,"text/html");
    else if(strstr(filename,".gif"))
            strcpy(filetype,"imgage/gif");
    else if(strstr(filename,".png"))
            strcpy(filetype,"imgage/png");
    else if(strstr(filename,".jpg"))
            strcpy(filetype,"imgage/jpeg");
    else
        strcpy(filetype,"text/plain");
    return;
}