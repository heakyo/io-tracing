CFLAGS = -O0 -g
LDFLAGS = -laio

all: main
	objdump -dS main > main.dis

install:
	cp -a main /usr/local/bin

uninstall:
	rm /usr/local/bin/main

main: main.o
	@ $(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

main.o: main.i
	@ $(CC) $(CFLAGS) -c -o $@ $<

%.i: %.c
	@ $(CC) -E -o $@ $<

.PHONY:clean
clean:
	@ rm -rf main *.i *.o *.dis
