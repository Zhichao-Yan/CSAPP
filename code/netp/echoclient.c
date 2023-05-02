#include "../csapp.h"

int main(int argc,char **argv)
{
    int clientfd;
    char *host,*port,buf[MAXLINE];
    rio_t rio;
    if(argc != 3)
    {
        fprintf(stderr,"usage: %s <host> <port>\n",argv[0]);
        exit(0);
    }
    host = argv[1];
    port = argv[2];
    clientfd = Open_clientfd(host,port);
    Rio_readinitb(&rio,clientfd);
    while(Fgets(buf,MAXLINE,stdin) != NULL) // 从标准输入读取字符串到buf
    {
        //write(clientfd,buf,strlen(buf)); //将buf中将最多strlen(buf)个字节写入套接字文件
        Rio_writen(clientfd,buf,strlen(buf));
        //read(clientfd,buf,MAXLINE);//从clientfd读取最多MAXLINE个字节到buf中
        Rio_readlineb(&rio,buf,MAXLINE); // 从服务器读取会送到行到buf
        Fputs(buf,stdout); // 将buf输出到标准输出
    }
    Close(clientfd);
    printf("bye!!%s\n",port);
    exit(0);
}
