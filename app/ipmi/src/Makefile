CFLAGS = -g -O0

all: main
        #objdump -dS main > main.dis

install:
	cp -a main /usr/local/bin

uninstall:
	rm /usr/local/bin/main

main: main.o
	$(CC) $(CFLAGS) -o $@ $< -lpthread -lisi_ipmi -lipmiutil

main.o: main.i
	$(CC) $(CFLAGS) -c -o $@ $<

%.i: %.c
	$(CC) -E -o $@ $<

.PHONY:clean
clean:
	rm -f main *.o *.i *.core
