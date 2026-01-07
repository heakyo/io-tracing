CFLAGS = -O0 -g

all: main
	#objdump -dS main > main.dis

install:
	cp -a main /usr/local/bin

uninstall:
	rm /usr/local/bin/main

main: main.c
	$(CC) $(CFLAGS) -o $@ $<

.PHONY:clean
clean:
	rm -rf main *.i *.o *.dis
