// 基于线程的并发echo服务器
#include "../csapp.h"


typedef struct data
{
    char port[MAXLINE];
    char hostname[MAXLINE];
    int connfd;
}data;

void echo(data A);
void* thread(void* vargp);

int main(int argc,char **argv)
{
    int listenfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    pthread_t tid;
    // usage
    if(argc != 2)
    {
        fprintf(stderr,"usage: %s <port>\n",argv[0]);
        exit(0);
    }

    listenfd = Open_listenfd(argv[1]);

    while(1)
    {
        clientlen = sizeof(struct sockaddr_storage);
        data A;
        A.connfd = Accept(listenfd,(SA*)&clientaddr,&clientlen);
        getnameinfo((SA*)&clientaddr,clientlen,A.hostname,MAXLINE,A.port,MAXLINE,0);
        printf("Connected to client:[%s-%s]\n",A.hostname,A.port);
        Pthread_create(&tid,NULL,thread,&A); //创建线程
        //printf("成功创建线程-%d\n",(int)tid);
    }
}

void echo(data A)
{
    size_t n;
    int bytecnt = 0;
    char buf[MAXLINE];
    rio_t rio;
    Rio_readinitb(&rio,A.connfd);
    while((n = Rio_readlineb(&rio,buf,MAXLINE)) != 0) // 从rio读取了n个字节在buf中
    {
        bytecnt += n;
        printf("%s",buf);
        printf("Server received %d bytes（total %d) from client:%s-%s\n",(int)n,bytecnt,A.hostname,A.port); // 显示收到了n个字节
        Rio_writen(A.connfd,buf,n); // 写回buf中n个字节到connfd
    }
    return;
}
void* thread(void* vargp)
{
    data A = *((data*)vargp);
    Pthread_detach(Pthread_self());//分离线程
    echo(A);
    Close(A.connfd);
    printf("See you %s!!\n",A.port);
    return NULL;
}