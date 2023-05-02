
#include "../csapp.h"

void echo(int connfd)
{
    size_t n;
    char buf[MAXLINE];
    rio_t rio;
    Rio_readinitb(&rio,connfd);
    while((n = Rio_readlineb(&rio,buf,MAXLINE)) != 0) // 从rio读取了n个字节在buf中
    {
        printf("server received %d bytes\n",(int)n); // 显示收到了n个字节
        printf("%s",buf);
        Rio_writen(connfd,buf,n); // 写回buf中n个字节到connfd
    }
}