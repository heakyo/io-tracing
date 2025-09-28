CFLAGS = -O0 -g
LDFLAGS = $(shell pkg-config fuse3 --cflags --libs)

HF_MNT_POINT = /mnt/hellofs

all: hellofs
	@objdump -dS hellofs > hellofs.dis

hellofs: hellofs.o
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

hellofs.o: hellofs.i
	@$(CC) $(CFLAGS) -c -o $@ $<

%.i: %.c
	@$(CC) -E -o $@ $<

check-dir:
	@if [ -d $(HF_MNT_POINT) ]; then \
		echo "The dir $(HF_MNT_POINT) exists"; \
	else \
		echo "The dir $(HF_MNT_POINT) doesn't exist"; \
		mkdir -p $(HF_MNT_POINT) || { echo "ERROR: Failed to create directory $(HF_MNT_POINT)"; exit 1; }; \
		echo "The dir $(HF_MNT_POINT) has been created"; \
	fi

mount-hellofs: hellofs check-dir
	@./hellofs /mnt/hellofs
	@echo "The dir $(HF_MNT_POINT) is mounted"

umount-hellofs:
	@df -Tha | grep hellofs > /dev/null || { echo "ERROR: The dir $(HF_MNT_POINT) is not mounted"; exit 1; }
	@echo "Umount dir $(HF_MNT_POINT)"
	@fusermount3 -u /mnt/hellofs

.PHONY:clean
clean:
	@rm -rf hellofs *.i *.o *.dis
