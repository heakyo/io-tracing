CFLAGS = -O0 -g

all: main
	objdump -dS main > main.dis

install:
	cp -a main /usr/local/bin

main: main.o
	$(CC) $(CFLAGS) -o $@ $< -laio

main.o: main.i
	$(CC) $(CFLAGS) -c -o $@ $<

%.i: %.c
	$(CC) -E -o $@ $<

.PHONY:clean
clean:
	rm main *.i *.o *.dis -rf
