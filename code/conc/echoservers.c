#include "../csapp.h"
// 基于I/O多路复用的并发echo服务器
typedef struct {
    int maxfd; // 在可读集合中最大的描述符
    fd_set read_set;// 可读集合
    fd_set ready_set; // 准备好读集合
    int nready; // select后返回的准备好读集合的大小
    int maxi; //clientfd的最大索引
    int clientfd[FD_SETSIZE];// 连接描述符的集合 FD_SETSIZE大小为1024
    rio_t clientrio[FD_SETSIZE];//上面集合中连接符描述符对应的读缓冲
}pool;

/* $end echoserversmain */
void init_pool(int listenfd,pool *p);
void add_client(int connfd,pool *p);
void check_clients(pool *p);
/* $begin echoserversmain */

int byte_cnt = 0; /* Counts total bytes received by server */

int main(int argc,char **argv)
{
    int listenfd,connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    static pool pool; // 静态全局变量pool
    // usage
    if(argc != 2)
    {
        fprintf(stderr,"usage: %s <port>\n",argv[0]);
        exit(0);
    }    
    listenfd = Open_listenfd(argv[1]);//打开一个监听描述符文件
    init_pool(listenfd,&pool);
    while(1)
    {
        pool.ready_set = pool.read_set;
        pool.nready = Select(pool.maxfd+1,&pool.ready_set,NULL,NULL,NULL);//检测输入事件
        if(FD_ISSET(listenfd,&pool.ready_set))// 检测到监听描述符
        {
            clientlen = sizeof(struct sockaddr_storage);
            connfd = Accept(listenfd,(SA*)&clientaddr,&clientlen);//接受一个客户端连接
            add_client(connfd,&pool);//把客户端添加到连接池里面
        }
        check_clients(&pool);//把来自每个准备好的已连接描述符的一个文本行回送回去
    }
}
/* $end echoserversmain */

// 初始化客户端池
void init_pool(int listenfd,pool *p)
{
    int i;
    p->maxi = -1;
    // -1表示
    for(i = 0; i< FD_SETSIZE; i++)
    {
        p->clientfd[i] = -1;//表示一个可用的槽位
    }
    p->maxfd = listenfd; // 初始可读集合最大描述符文件
    FD_ZERO(&p->read_set);//清空可读集合
    FD_SET(listenfd,&p->read_set);//初识把监听描述符文件加入可读集合，是读集合中唯一的描述符
    return;
}
/* $end init_pool */

/* $begin add_client */
// 添加新的客户端到活动客户端池中
void add_client(int connfd,pool *p)
{
    int i;
    p->nready--;// 读取了监听描述符，所以递减1
    for(i = 0; i < FD_SETSIZE; i++)
    {
        if(p->clientfd[i] < 0) //有空槽
        {
            p->clientfd[i] = connfd;//放入空槽中
            Rio_readinitb(&p->clientrio[i],connfd);//初始化连接描述符文件对应的缓冲区
            FD_SET(connfd,&p->read_set);//将连接描述符放入可读的描述符文件集合中
            if(connfd > p->maxfd) // 更新最大文件描述符
            {
                p->maxfd = connfd;
            }
            if(i > p->maxi) // 更新clientfd数组的最大索引
            {
                p->maxi = i;
            }
            break;
        }
    }
    if(i == FD_SETSIZE) //说明没有空槽位，太多客户端连接了
        app_error("add_client error:Too many clients");
}

void check_clients(pool *p)
{
    int i,connfd,n;
    char buf[MAXLINE];
    rio_t rio;
    for(i = 0;(i<=p->maxi)&&(p->nready>0);i++)
    {
        connfd = p->clientfd[i];
        rio = p->clientrio[i];
        if((connfd>0)&&(FD_ISSET(connfd,&p->ready_set)))
        {
            p->nready--;//读取一个，因此要减1
            if(( n = Rio_readlineb(&rio,buf,MAXLINE))!=0)//从rio中读取一行到buf中
            {
                byte_cnt += n;
                printf("%s",buf);
                printf("Server received %d/%d(total bytes) on fd %d\n",n,byte_cnt,connfd);
                Rio_writen(connfd,buf,n);//从buf中写回到connfd
            }else{
                Close(connfd); // 把连接描述符文件connfd关闭
                printf("goodbye to fd-%d\n",connfd);
                FD_CLR(connfd,&p->read_set);//从可读的描述符集合中删除connfd
                p->clientfd[i] = -1;//重新把槽置空
            }
        }
    }
}
/* $end check_clients */

