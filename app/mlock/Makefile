CFLAGS = -O0 -g

all: mlock
	objdump -dS mlock > mlock.dis

install:
	cp -a mlock /usr/local/bin

uninstall:
	rm /usr/local/bin/mlock

mlock: mlock.o
	$(CC) $(CFLAGS) -o $@ $< -laio

mlock.o: mlock.i
	$(CC) $(CFLAGS) -c -o $@ $<

%.i: %.c
	$(CC) -E -o $@ $<

.PHONY:clean
clean:
	rm mlock *.i *.o *.dis -rf
