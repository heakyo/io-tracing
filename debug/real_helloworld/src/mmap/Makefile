CC = cc
CFLAGS = -g0 -O0
TEST_FILE=./gm0_mnt/data8k

all: main

#main: main.o mkdata
main: main.o
	$(CC) $(CFLAGS) -o $@ main.o

main.o:main.c
	$(CC) $(CFLAGS) -c -o $@ main.c

.PHONY: all clean mkdata rhwd
clean:
	rm -rf main main.o $(TEST_FILE)

mkdata:
	dd if=/dev/urandom of=$(TEST_FILE) bs=4k count=2

rhwd: main
	./rhw.d -c './main'
	#rm -rf $(TEST_FILE)

rhw_sysread: main
	./rhw_sysread.d -c './main'

rhw_intr: main
	./rhw_intr.d -c './main'

rhw_cam: main
	./rhw_cam.d -c './main'
