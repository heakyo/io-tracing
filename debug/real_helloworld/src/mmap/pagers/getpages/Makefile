CC = cc
CFLAGS = -g0 -O0
DATA=mnt/data

all: main

main: main.o
	$(CC) $(CFLAGS) -o $@ main.o

main.o:main.c
	$(CC) $(CFLAGS) -c -o $@ main.c

.PHONY: all clean rhw_getpages_pread rhw_getpages_sysmap gen_data
clean:
	rm -rf main main.o $(DATA)

gen_data:
	dd if=/dev/urandom of=data bs=1k count=16

rhw_getpages_pread: main
	mount /dev/ada0p19 mnt
	fsync $(DATA)
	./rhw_getpages.d -c './main -r'
	umount mnt

# this will have page fault(vm_fault)
rhw_getpages_sysmap: main
	mount /dev/ada0p19 mnt
	fsync $(DATA)
	./rhw_getpages.d -c './main -s'
	umount mnt
