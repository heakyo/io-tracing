#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define TEST_FILE "gm0_mnt/data8k"
#define RW_TEST_FILE "gm0_mnt/data_rw8k"

#define BUFSZ 8192

//unsigned char a[4096] = {0x55};
unsigned char a[4096];

static void
mmap_test(void)
{
	int fd;
	struct stat statbuf;
	char *mapped_region;
	size_t filesize;
	int ret;

	fd = open(TEST_FILE, O_RDONLY);
	assert(fd != -1);

	ret = fstat(fd, &statbuf);
	assert(ret != -1);

	//getchar();

	filesize = statbuf.st_size;
	printf("memory map %s\n", TEST_FILE);
	mapped_region = (char *)mmap(NULL, filesize, PROT_READ, MAP_PRIVATE, fd, 0);
	assert(mapped_region != MAP_FAILED);

	//getchar();

	printf("v:%x\n", mapped_region[0]);
	printf("memory unmap %s\n", TEST_FILE);
	ret = munmap(mapped_region, filesize);
	assert(ret != -1);

	close(fd);
}

static void
dump_data(unsigned char *buf, unsigned int nbytes)
{
	for (int i = 0; i < nbytes; i++) {
		if (i % 16 == 0) {
			printf("\n%08x: ", i);
		}
		printf("%02x ", buf[i]);
	}

	printf("\n");
}

static void
rw_test_create(unsigned char *buf, int size)
{
	int fd;
	int ret;

	fd = open(RW_TEST_FILE, O_CREAT | O_RDWR | O_DIRECT);
	assert(fd != -1);

	ret = write(fd, buf, size);
	assert(ret != -1);

	close(fd);
}

static void
rw_test_read(unsigned char *buf, int size)
{

	int fd;
	int ret;

	fd = open(RW_TEST_FILE, O_RDWR);
	assert(fd != -1);

	ret = read(fd, buf, size);
	assert(ret != -1);

	close(fd);
}

static void
rw_test(void)
{
	unsigned char buf[BUFSZ];

	memset(buf, 0xA5, sizeof(buf));

	rw_test_create(buf, sizeof(buf));

	memset(buf, 0x0, sizeof(buf));
	rw_test_read(buf, sizeof(buf));

	dump_data(buf, 32);
}

int
main(int argc, char *args[])
{
	printf("Real Hello World\n");

	//mmap_test();
	rw_test();

	//getchar();

	return 0;
}
