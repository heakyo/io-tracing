CFLAGS = -O0 -g

all: main
	#objdump -dS main > main.dis

install:
	cp -a main /usr/local/bin

uninstall:
	rm /usr/local/bin/main

all: main ptrace_attach_example example

main: main.o
	$(CC) $(CFLAGS) -o $@ $<

ptrace_attach_example: ptrace_attach_example.o
	$(CC) $(CFLAGS) -o $@ $<

example: example.o
	$(CC) $(CFLAGS) -o $@ $<

%.o: %.i
	$(CC) $(CFLAGS) -c -o $@ $<

%.i: %.c
	$(CC) -E -o $@ $<

.PHONY:clean
clean:
	rm -rf main ptrace_attach_example example
	rm -rf *.i *.o *.dis *.gcda *.gcno
