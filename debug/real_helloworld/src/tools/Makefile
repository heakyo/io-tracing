CC = cc
CFLAGS = -Wall -g0 -O0

all: main
	./main data-in data-out
	hexdump -C data-out

main: main.o
	$(CC) $(CFLAGS) -o $@ main.o
	dd if=/dev/urandom of=data-in bs=128 count=1

main.o:main.c
	$(CC) $(CFLAGS) -c -o $@ main.c

.PHONY: all clean
clean:
	@rm -rf main main.o data-in data-out
