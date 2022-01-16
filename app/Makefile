CFLAGS = -O0 -g

all: main
	objdump -dS main > main.dis
	#cp -a main /usr/local/bin

main: main.o
	$(CC) -o $@ $<

main.o: main.i
	$(CC) $(CFLAGS) -c -o $@ $<

%.i: %.c
	$(CC) -E -o $@ $<

.PHONY:clean
clean:
	rm main *.i *.o *.dis -rf
