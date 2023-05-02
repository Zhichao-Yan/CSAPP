#include "../csapp.h"

static int byte_cnt;
static sem_t *mutex;//改有名信号量

static void init_echo_cnt(void)
{
    mutex = sem_open("/mutex1",O_CREAT, S_IRUSR | S_IWUSR, 1);
    byte_cnt = 0;
}

void echo_cnt(int connfd)
{
    int n;
    char buf[MAXLINE];
    rio_t rio;
    //总是被初始化为这个PTHREAD_ONCE_INIT宏，意味着只有一次线程初始化
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    // 动态初始化多个线程共享的全局变量/或静态全局
    // 任意线程第一次使用onc_control调用pthread_once时，它调用init_echo_cnt函数初始化初始化全局变量
    // 任意线程第二次调用pthread_once时，啥也不改变
    Pthread_once(&once_control,init_echo_cnt);
    Rio_readinitb(&rio,connfd);//初始化RIO缓冲
    // 从缓冲中读取数据到buf
    while((n = Rio_readlineb(&rio,buf,MAXLINE)) != 0)
    {
        P(mutex);
        byte_cnt += n;
        printf("server received %d(total %d) bytes on fd-%d\n",n,byte_cnt,connfd);
        V(mutex);
        Rio_writen(connfd,buf,n);// 从buf写回到connfd中
    }
}