CFLAGS = -O0 -g

all: main
	objdump -dS main > main.dis

record:
	perf record -g ./main

flame-graph: main record
	perf script | ./bin/stackcollapse-perf.pl | ./bin/flamegraph.pl > main.fg.svg
	cp main.fg.svg /home/marvin/docker/docker-server/caddy/src/linux-perf/flame-graph

report: main record
	perf report -g --no-child --stdio

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
	@rm -rf main *.i *.o *.dis *.svg *. perf.data*
