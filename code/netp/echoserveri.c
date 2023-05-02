#include "../csapp.h"

// echo服务器迭代版本
void echo(int connfd);

int main(int argc,char **argv)
{
    int listenfd,connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    char client_hostname[MAXLINE],client_port[MAXLINE];

    if(argc != 2)
    {
        fprintf(stderr,"usage: %s <port>\n",argv[0]);
        exit(0);
    }
    listenfd = Open_listenfd(argv[1]);
    while(1)
    {
        clientlen = sizeof(struct sockaddr_storage);
        // 用监听套接字接受连接请求，并且返回客户端套接字的地址和长度
        connfd = Accept(listenfd,(SA*)&clientaddr,&clientlen); 
        getnameinfo((SA*)&clientaddr,clientlen,client_hostname,MAXLINE,client_port,MAXLINE,0);
        printf("Connected to (%s,%s)\n",client_hostname,client_port);
        echo(connfd);
        Close(connfd);
        printf("goodbye to (%s,%s)\n",client_hostname,client_port);
    }
    exit(0);
}