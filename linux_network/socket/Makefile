CC = gcc
CFLAGS = -O0 -g

FILES = client server

all: client server
	@objdump -dS client > client.dis
	@objdump -dS server > server.dis
	@#cp -a client /usr/local/bin

client: client.o
	@$(CC) $(CFLAGS) -o $@ $<

server: server.o
	@$(CC) $(CFLAGS) -o $@ $<

%.o: %.i
	@$(CC) $(CFLAGS) -c -o $@ $<

%.i: %.c
	@$(CC) -E -o $@ $<

.PHONY:clean
clean:
	@rm client server *.i *.o *.dis -rf
