#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define TEST_FILE "gm0_mnt/data8k"

void mmap_test()
{
	int fd;
	struct stat statbuf;
	void *mapped_region;
	size_t filesize;
	int ret;

	fd = open(TEST_FILE, O_RDONLY);
	assert(fd != -1);

	ret = fstat(fd, &statbuf);
	assert(ret != -1);

	//getchar();

	filesize = statbuf.st_size;
	printf("memory map %s\n", TEST_FILE);
	mapped_region = mmap(NULL, filesize, PROT_READ, MAP_PRIVATE, fd, 0);
	assert(mapped_region != MAP_FAILED);

	//getchar();

	printf("memory unmap %s\n", TEST_FILE);
	ret = munmap(mapped_region, filesize);
	assert(ret != -1);

	close(fd);
}

int main(int argc, char *args[])
{
	printf("Real Hello World\n");

	mmap_test();

	//getchar();

	return 0;
}
