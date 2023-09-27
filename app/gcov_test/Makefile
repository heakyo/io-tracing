CFLAGS = -O0 -g -fprofile-arcs -ftest-coverage

all: main
	#objdump -dS main > main.dis

install:
	cp -a main /usr/local/bin

uninstall:
	rm /usr/local/bin/main

main: main.o
	$(CC) $(CFLAGS) -o $@ $<

main.o: main.i
	$(CC) $(CFLAGS) -c -o $@ $<

%.i: %.c
	$(CC) -E -o $@ $<

.PHONY:clean
clean:
	rm -rf main *.i *.o *.dis *.gcda *.gcno
