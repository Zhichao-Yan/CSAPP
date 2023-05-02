CC = gcc
CFLAGS = -O2 -Wall -I..

echoserveri:echoserveri.c ../src/csapp.o echo.c
	$(CC) $(CFLAGS) -o echoserveri echoserveri.c ../src/csapp.o echo.c

echoclient:echoclient.c ../src/csapp.o
	$(CC) $(CFLAGS) -o echoclient echoclient.c ../src/csapp.o

hostinfo:hostinfo.c 
	$(CC) $(CFLAGS) hostinfo.c -o hostinfo

../src/csapp.o:../src/csapp.c
	cd ../src/&&make

all:echoserveri echoclient hostinfo