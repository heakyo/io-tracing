CFLAGS = -O0 -g
LDFLAGS = -laio

all: main pwritev
	objdump -dS main > main.dis

install:
	cp -a main /usr/local/bin

uninstall:
	rm /usr/local/bin/main

pwritev: pwritev.o
	@ $(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

main: main.o
	@ $(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

%.o: %.i
	@ $(CC) $(CFLAGS) -c -o $@ $<

%.i: %.c
	@ $(CC) -E -o $@ $<

.PHONY:clean
clean:
	@ rm -rf main *.i *.o *.dis
