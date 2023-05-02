CC = gcc
# -Wall 生成所有警告信息。
# -02 进一步优化--优化级别
# -I .. 将 ..的上层作为寻找头文件的目录
CFLAGS = -O2 -Wall -I..

all: adder

adder: adder.c
	$(CC) $(CFLAGS) -o adder adder.c

clean:
	rm -f adder *~
