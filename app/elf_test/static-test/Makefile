#CFLAGS = -O0 -g -fpie -pie
CFLAGS = -O0 -g
CC=gcc

all: main
	objdump -dS main > main.dis
	objdump -dS sub.o > sub.o.dis
	objdump -dS main.o > main.o.dis

install:
	cp -a main /usr/local/bin

uninstall:
	rm /usr/local/bin/main

main: main.o sub.o
	$(CC) $(CFLAGS) -o $@ $^ -laio

%.o: %.i
	$(CC) $(CFLAGS) -c -o $@ $^

%.i: %.c
	$(CC) -E -o $@ $^

.PHONY:clean
clean:
	rm main *.i *.o *.dis -rf
