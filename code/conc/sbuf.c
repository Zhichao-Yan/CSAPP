#include "../csapp.h"
#include "sbuf.h"

void sbuf_init(sbuf_t *sp,int n)
{
    sp->buf = (int*)Calloc(n,sizeof(int));
    sp->n = n;
    sp->front = sp->rear = 0;
    //Sem_init(&sp->mutex,0,1);
    //Sem_init(&sp->slots,0,n);
    //Sem_init(&sp->items,0,0);
    //sp->mutex = 1;
    //sp->slots = n;
    //sp->items = 0; 
    sem_t *mutex = sem_open("/mutex",O_CREAT, S_IRUSR | S_IWUSR, 1);
    sp->mutex = mutex;
    sp->slots = sem_open("/slots",O_CREAT, S_IRUSR | S_IWUSR, n);
    sp->items = sem_open("/items",O_CREAT, S_IRUSR | S_IWUSR, 0);
    return;
}

void sbuf_deinit(sbuf_t *sp)
{
    Free(sp->buf);
}
void sbuf_insert(sbuf_t *sp,int item)
{
    P(sp->slots);
    P(sp->mutex);
    sp->buf[(++sp->rear)%(sp->n)] = item;
    V(sp->mutex);
    V(sp->items);
}
int sbuf_remove(sbuf_t *sp)
{
    int item;
    P(sp->items);
    P(sp->mutex);
    item = sp->buf[(++sp->front)%(sp->n)];
    V(sp->mutex);
    V(sp->slots);
    return item;
}