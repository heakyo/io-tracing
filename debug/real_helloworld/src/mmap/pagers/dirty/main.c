#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define BUFSZ (32*1024)
static int
file_mmap(void)
{
	unsigned char *p1;
	char buf[BUFSZ];
	int fd;
	int ret;
	size_t length;
	struct stat sb;

	fd = open("data32k", O_RDWR | O_CREAT | O_TRUNC);
	assert(fd != -1);

        memset(buf, 0x15, sizeof(buf));
        ret = write(fd, buf, sizeof(buf));
        assert(ret != -1);

	assert(fstat(fd, &sb) != -1);
	length = sb.st_size;
	printf("length:%ld\n", length);

	printf("Memory mapping....\n");
	p1 = mmap(NULL, length, PROT_READ | PROT_WRITE,
			MAP_SHARED,
			fd, 0);
	assert(p1 != MAP_FAILED);

	printf("Writing....\n");
	memset(p1, 0xb5, 4096);
	msync(p1, length, MS_SYNC);

	printf("Memory unmapping....\n");
	ret = munmap(p1, length);
	assert(ret != -1);

	close(fd);

	return 0;
}

int
main(int argc, char *argv[])
{
	file_mmap();

	return 0;
}
