CFLAGS = -fPIC
CC=gcc

all: main
	objdump -dS main > main.dis
	objdump -dS libb.so > libb.so.dis
	objdump -dS liba.so > liba.so.dis

install:
	cp -a main /usr/local/bin

uninstall:
	rm /usr/local/bin/main

main: main.o liba.so libb.so
	$(CC) $(CFLAGS) -o $@ $< -ldl -la -lb -L./

liba.so: a.o libb.so
	$(CC) -fPIC --shared -o $@ $< -lb -L./

libb.so: b.o
	$(CC) -fPIC --shared -o $@ $<

%.o: %.i
	$(CC) $(CFLAGS) -c -o $@ $^

%.i: %.c
	$(CC) -E -o $@ $^

.PHONY:clean
clean:
	rm main *.i *.o *.dis liba.so libb.so -rf
