CFLAGS = -O0 -g

all: hellofs
	@ objdump -dS hellofs > hellofs.dis

hellofs: hellofs.o
	@ $(CC) $(CFLAGS) -o $@ $<

hellofs.o: hellofs.i
	@ $(CC) $(CFLAGS) -c -o $@ $<

%.i: %.c
	@ $(CC) -E -o $@ $<

.PHONY:clean
clean:
	@ rm -rf hellofs *.i *.o *.dis
