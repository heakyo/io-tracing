CFLAGS = -O0 -g -fprofile-arcs -ftest-coverage

INFO = test.info
IPV4 = $(shell ifconfig bge0 | grep -w inet | column -t | awk '{print $$2}')

all: main
	#objdump -dS main > main.dis

cov: main
	./main
	lcov -c -d . -o $(INFO) --rc lcov_branch_coverage=1
	genhtml --branch-coverage -o result $(INFO)

pyhttp:
	@echo "http server: $(IPV4):8888"
	python2 -m SimpleHTTPServer 8888

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
	rm -rf main *.i *.o *.dis *.gcda *.gcno *.gcov result $(INFO)
