CC = cc
CFLAGS = -Wall -g0 -O0
TEST_FILE=data8k

all: main

main: main.o mkdata
	$(CC) $(CFLAGS) -o $@ main.o

main.o:main.c
	$(CC) $(CFLAGS) -c -o $@ main.c

.PHONY: all clean mkdata rwhd
clean:
	rm -rf main main.o $(TEST_FILE)

mkdata:
	dd if=/dev/urandom of=$(TEST_FILE) bs=8k count=1

rwhd: main
	./rhw.d -c './main'
	rm -rf $(TEST_FILE)
