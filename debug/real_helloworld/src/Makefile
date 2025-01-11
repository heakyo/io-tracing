CC = cc
CFLAGS = -Wall -g0 -O0

all: main

main: main.o
	@$(CC) $(CFLAGS) -o $@ main.o

main.o:main.c
	@$(CC) $(CFLAGS) -c -o $@ main.c

.PHONY: all clean
clean:
	@rm -rf main main.o
