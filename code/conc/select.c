#include "../csapp.h"

void echo(int connfd,char *port);

void command(void);

int main(int argc,char **argv)
{
    int listenfd,connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    char client_hostname[MAXLINE],client_port[MAXLINE];
    fd_set read_set,ready_set;

    // usage
    if(argc != 2)
    {
        fprintf(stderr,"usage: %s <port>\n",argv[0]);
        exit(0);
    }
    listenfd = Open_listenfd(argv[1]);// 打开监听描述符文件
    FD_ZERO(&read_set);// 创建空的可读集合
    FD_SET(STDIN_FILENO,&read_set);//及那个标准输入加入读集合
    FD_SET(listenfd,&read_set);//将监听描述符文件加入读集合

    while(1)
    {
        ready_set = read_set;
        Select(listenfd+1,&ready_set,NULL,NULL,NULL);
        if(FD_ISSET(STDIN_FILENO,&ready_set)) // 宏指令来确定标准输入是否准备好可以读
            command(); //解析响应命令
        if(FD_ISSET(listenfd,&ready_set)) // 宏指令来确定监听描述符文件是否准备好可以读
        {
            clientlen = sizeof(struct sockaddr_storage);
            // 调用accept得到一个连接描述符文件，用来读写
            connfd = Accept(listenfd,(SA*)&clientaddr,&clientlen);
            getnameinfo((SA*)&clientaddr,clientlen,client_hostname,MAXLINE,client_port,MAXLINE,0);
            echo(connfd,client_port);
            Close(connfd);
            printf("goodbye to %s\n",client_port);
        }
    }
}

void echo(int connfd,char *port)
{
    size_t n;
    static int bytecnt = 0;
    char buf[MAXLINE];
    rio_t rio;
    Rio_readinitb(&rio,connfd);
    while((n = Rio_readlineb(&rio,buf,MAXLINE)) != 0) // 从rio读取了n个字节在buf中
    {
        bytecnt += n - 1;
        printf("%s",buf);
        printf("current process %d received %d/%d total bytes from %s\n",getpid(),(int)(n-1),bytecnt,port); // 显示收到了n个字节
        Rio_writen(connfd,buf,n); // 写回buf中n个字节到connfd
    }
    return;
}
void command(void)
{
    char buf[MAXLINE];
    if(!Fgets(buf,MAXLINE,stdin)) // 从标准输入文件读取字符串
        exit(0);// 读到EOF则进程退出
    printf("stdin:%s",buf);
    return;
}