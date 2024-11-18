# Determin OS
OS := $(shell uname -s)

CFLAGS = -O0 -g -fprofile-arcs -ftest-coverage

INFO = test.info

ifeq ($(OS), FreeBSD)
	IPV4 = $(shell ifconfig bge0 | grep -w inet | awk '{print $2}' | cut -d/ -f1)
else ifeq ($(OS), Linux)
	IPV4 = $(shell ip a show eth0 | grep -w inet | awk '{print $2}' | cut -d/ -f1)
else
	IPV4 = "0.0.0.0"
endif

all: main
	#objdump -dS main > main.dis

cov: main
	./main
	lcov -c -d . -o $(INFO) --rc lcov_branch_coverage=1
	genhtml --branch-coverage -o result $(INFO)

pyhttp:
	@echo "http server: $(IPV4):8888"
	python -m http.server 8888

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
