CC = gcc
CFLAGS = -O2 -Wall -I..

echoserverp:echoserverp.c ../src/csapp.o
	$(CC) $(CFLAGS) -o echoserverp echoserverp.c ../src/csapp.o

select:select.c ../src/csapp.o
	$(CC) $(CFLAGS) -o select select.c ../src/csapp.o

echoservers:echoservers.c ../src/csapp.o
	$(CC) $(CFLAGS) -o echoservers echoservers.c ../src/csapp.o

echoservert:echoservert.c ../src/csapp.o
	$(CC) $(CFLAGS) -o echoservert echoservert.c ../src/csapp.o

sharing:sharing.c ../src/csapp.o
	$(CC) $(CFLAGS) -o sharing sharing.c ../src/csapp.o

badcnt:badcnt.c ../src/csapp.o
	$(CC) $(CFLAGS) -o badcnt badcnt.c ../src/csapp.o

echoserver:echoserver-pre.c sbuf.c echo-cnt.c ../src/csapp.o
	$(CC) $(CFLAGS) -g -o echoserver echoserver-pre.c sbuf.c echo-cnt.c ../src/csapp.o

all:echoserverp echoservers select echoservert