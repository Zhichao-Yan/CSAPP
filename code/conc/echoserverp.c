#include "../csapp.h"

// 基于进程的并发echo服务器
void echo(int connfd,char *port);

void sigchld_handler(int sig)
{
    while(waitpid(-1,0,WNOHANG) > 0)
        ;
    return;
}

int main(int argc,char **argv)
{
    int listenfd,connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    char client_hostname[MAXLINE],client_port[MAXLINE];

    // usage
    if(argc != 2)
    {
        fprintf(stderr,"usage: %s <port>\n",argv[0]);
        exit(0);
    }

    Signal(SIGCHLD,sigchld_handler);
    listenfd = Open_listenfd(argv[1]);
    while(1)
    {
        clientlen = sizeof(struct sockaddr_storage);
        connfd = Accept(listenfd,(SA*)&clientaddr,&clientlen);
        getnameinfo((SA*)&clientaddr,clientlen,client_hostname,MAXLINE,client_port,MAXLINE,0);
        printf("Connected to (%s,%s)\n",client_hostname,client_port);
        if(Fork() == 0)
        {
            Close(listenfd);
            echo(connfd,client_port);
            Close(connfd);
            printf("goodbye to %s\n",client_port);
            exit(0);
        }
        Close(connfd);
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