CC = gcc
CFLAGS = -O2 -Wall -I..

# This flag includes the Pthreads library on a Linux box.
# Others systems will probably require something different.
LIB = -lpthread

all: tiny cgi

tiny: tiny.c ../src/csapp.o
	$(CC) $(CFLAGS) -o tiny tiny.c ../src/csapp.o $(LIB)

../src/csapp.o: ../src/csapp.c
	echo "compiling csapp.c";cd ../src;make;

cgi:
	cd cgi-bin;make

clean:
	rm -f *.o tiny *~
	(cd cgi-bin; make clean)
