CC = cc
CFLAGS = -g0 -O0

all: main

main: main.o
	$(CC) $(CFLAGS) -o $@ main.o

main.o:main.c
	$(CC) $(CFLAGS) -c -o $@ main.c

.PHONY: all clean rhw_dirty
clean:
	rm -rf main main.o data32k

rhw_dirty: main
	./rhw_dirty.d -c './main -w'
