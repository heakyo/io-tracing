#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>

//#define RW_TEST_FILE "data_rw32k"
#define RW_TEST_FILE "gm0_mnt/data_rw32k"
#define TEST_FILE RW_TEST_FILE

// the buf size 8k can make the flag B_CACHE unset
#define BUFSZ (1024*8)

//unsigned char a[4096] = {0x55};
unsigned char a[4096];

static int
mmap_test_anon(void)
{
	pid_t pid;
	char *mapped_region;
	size_t size = 4096*1; // One memory page
	int ret;

	//getchar();
	mapped_region = mmap(NULL, size, PROT_READ | PROT_WRITE,
			MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	assert(mapped_region != MAP_FAILED);

	// Use the memory
	//strcpy((char *)mapped_region, "Hello from mmap!");
	//printf("%s\n", (char *)mapped_region);

	printf("Parent -- Writing 'A' to memory\n");
	memset(mapped_region, 'A', size);  // Parent writes first

	pid = fork();

	if (pid < 0) {
		perror("fork");
		exit(1);
	}

	if (pid == 0) {
		// Child process
		printf("Child -- Reading memory before write. %c\n", mapped_region[0]);
		printf("Child -- Writing 'B' to memory (triggers Copy-On-Write!)\n");
		mapped_region[0] = 'B';
		printf("Child -- Reading memory after write. %c\n", mapped_region[0]);

		sleep(3);

	//getchar();
	ret = munmap(mapped_region, size);
	assert(ret != -1);

		printf("Child -- exiting...\n");
		exit(0);
	} else {
		// Parent process
		sleep(1); // Wait for child to modify memory
		printf("Parent -- Reading memory:%c\n", mapped_region[0]);

		wait(NULL);
	}

	// Unmap when done
	printf("Parent -- unmapping memory\n");
	ret = munmap(mapped_region, size);
	printf("Parent -- ret:%d\n", ret);
	assert(ret != -1);

	return 0;
}

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

	/* Comment out this line to set the flag B_CACHE */
	rw_test_create(buf, sizeof(buf));

	//printf("buf:%p nbytes:%ld\n", buf, sizeof(buf));
	memset(buf, 0x0, sizeof(buf));
	rw_test_read(buf, sizeof(buf));

	//dump_data(buf, 32);
}

static void
sysread_test(void)
{
	unsigned char buf[BUFSZ];

	memset(buf, 0x0, sizeof(buf));
	rw_test_read(buf, sizeof(buf));
}

int
main(int argc, char *args[])
{
	printf("Real Hello World\n");

	mmap_test_anon();
	//mmap_test();
	//sysread_test();
	//rw_test();

	//getchar();

	return 0;
}
