CFLAGS = -O0 -g

all: aio_main
	objdump -dS aio_main > aio_main.dis

install:
	cp -a aio_main /usr/local/bin

aio_main: aio_main.o
	$(CC) $(CFLAGS) -o $@ $< -laio

aio_main.o: aio_main.i
	$(CC) $(CFLAGS) -c -o $@ $<

%.i: %.c
	$(CC) -E -o $@ $<

.PHONY:clean
clean:
	rm aio_main *.i *.o *.dis -rf
