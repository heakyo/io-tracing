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
	char *p1;
	size_t sz = 4096*256;
	int ret;

	p1 = mmap(NULL, sz, PROT_READ | PROT_WRITE,
			MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	assert(p1 != MAP_FAILED);
	printf("Reading p1(%p) --- %d\n", p1, p1[0]);

	printf("Writing 'A' to p1(%p)\n", p1);
	memset(p1, 'A', 4096);  // Parent writes first
	printf("Reading p1(%p) --- %c\n", p1, p1[0]);

	ret = munmap(p1, sz);
	assert(ret != -1);

	return 0;
}

#define WRBUFSZ (32*1024)
static int
file_mmap(bool wflag)
{
	unsigned char *p1;
	int fd;
	char buf[WRBUFSZ];
	int ret;
	size_t length;
	struct stat sb;

	fd = open("data32k", O_RDWR | O_CREAT | O_TRUNC);
	assert(fd != -1);

	memset(buf, 0x15, sizeof(buf));
	ret = write(fd, buf, sizeof(buf));
	assert(ret != -1);

if (1) {
	ret = fstat(fd, &sb);
	assert(ret != -1);

	length = sb.st_size;
	printf("length:%ld\n", length);

	printf("Memory mapping....\n");
	p1 = mmap(NULL, length, PROT_READ | PROT_WRITE,
			MAP_SHARED,
			//MAP_PRIVATE,
			fd, 0);
	assert(p1 != MAP_FAILED);

	printf("Stop....\n");

	if (wflag) {
		printf("Writing....\n");
		memset(p1, 0xb5, 4096*2);
		msync(p1, length, MS_SYNC);
	}

	printf("Reading....\n");
	printf("%08x\n", p1[0]);

	printf("Stop....\n");

	printf("Memory unmapping....\n");
	ret = munmap(p1, length);
	assert(ret != -1);
}
	close(fd);

	return 0;
}

static int
mmap_test_file_fork(void)
{
	pid_t pid;
	char *p1, *p2;
	int fd;
	int ret;
	int res = 4096*1;
	size_t length;
	struct stat sb;

	fd = open("data16k", O_RDWR);
	assert(fd != -1);

	ret = fstat(fd, &sb);
	assert(ret != -1);
	length = sb.st_size;
	//length = 4096*3;
	printf("sb:size:%ld\n", sb.st_size);

	p1 = mmap(NULL, length, PROT_READ | PROT_WRITE,
			MAP_PRIVATE, fd, 0);
	assert(p1 != MAP_FAILED);
	printf("Parent -- Reading p1(%p) --- 0x%x\n", p1, p1[0]);

	printf("Parent -- Writing 'A' to p1(%p)\n", p1);
	//memset(p1, 'A', res); // This will affect the number of resident pages.
	printf("Parent -- Reading p1(%p) --- [0]:%c--[%d]:%c\n", p1, p1[0], res-1, p1[res-1]);


	//pid = fork();
	pid = 1;
	assert(pid >= 0);

	if (pid == 0) {

		size_t unmap_len = 4096*1;

		printf("Child -- Writing 'C' to p1(%p)\n", p1);
		memset(p1, 'C', unmap_len);
		printf("Child -- Reading p1(%p) --- %c\n", p1, p1[0]);

		printf("Child -- Writing 'D' to p1(%p)\n", p1);
		//memset(p1, 'D', 4096*1);
		printf("Child -- Reading p1(%p) --- %c\n", p1, p1[0]);

		printf("Child -- Writing 'E' to p1(%p)\n", p1);
		//memset(p1, 'E', 4096*1);
		printf("Child -- Reading p1(%p) --- %c\n", p1, p1[0]);

		sleep(2);
		printf("Child -- munmap p1:%p\n", p1);
		ret = munmap(p1, length);
		assert(ret != -1);

	} else {

		p2 = p1 + 4096;
		printf("Parent -- Writing 'B' to p2(%p)\n", p2);
		memset(p2, 'B', res); // This will affect the number of resident pages.
		printf("Parent -- Reading p2(%p) --- [0]:%c--[%d]:%c\n", p2, p2[0], res-1, p2[res-1]);

		//sleep(1);
		ret = munmap(p1, length);

		printf("Parent -- Waiting for child to exit\n");
		//wait(NULL);
	}

	printf("Parent -- munmap p1:%p\n", p1);
	//ret = munmap(p1, length);
	ret = 0;
	assert(ret != -1);

	close(fd);

	return 0;
}

static int
mmap_test_anon_fork(void)
{
	pid_t pid;
	char *mapped_region, *p1, *p2;
	size_t size = 4096*512;
	size_t sz = 4096*1024;
	int ret;

	printf("Parent -- before memory mapping...\n");
	//getchar();

	p1 = mmap(NULL, sz, PROT_READ | PROT_WRITE,
			MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	assert(p1 != MAP_FAILED);

	p2 = mmap(NULL, sz, PROT_READ | PROT_WRITE,
			MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	assert(p2 != MAP_FAILED);

	printf("Parent -- memory mapping...\n");
	mapped_region = mmap(NULL, size, PROT_READ | PROT_WRITE,
			MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	assert(mapped_region != MAP_FAILED);

	printf("\tp1:%p\n\tp2:%p\n\tmr:%p\n",
		p1, p2, mapped_region);
///////////////////////////////////////////////////////////////////

	// Use the memory
	printf("Parent -- Writing 'A' to p1\n");
	memset(p1, 'A', sz);  // Parent writes first
	printf("Parent -- Writing 'B' to p2\n");
	memset(p2, 'B', sz);  // Parent writes first
	printf("Parent -- Writing 'C' to memory mapped_region\n");
	memset(mapped_region, 'C', size);  // Parent writes first
///////////////////////////////////////////////////////////////////

	/*unmap p2 to make a hole between p1 and mapped_region*/
	ret = munmap(p2, sz);
	assert(ret != -1);
///////////////////////////////////////////////////////////////////

	printf("Parent -- before forking child. Waiting 3 seconds\n");
	sleep(3);

	pid = fork();
	//pid = 1;
	if (pid < 0) {
		perror("fork");
		exit(1);
	}

	if (pid == 0) {

		char c = 'D';

		// Child process
		printf("Child -- Reading memory before writing. '%c'\n", mapped_region[0]);
		printf("Child -- Writing '%c' to memory (triggers Copy-On-Write!)\n", c);
		mapped_region[0] = c;
		memset(mapped_region, c, size);
		printf("Child -- Reading memory after writing. %c\n", mapped_region[0]);

		//getchar();
		printf("Child -- before munmapping mapped_region\n");
		sleep(3);
		ret = munmap(mapped_region, size);
		assert(ret != -1);

		printf("Child -- exiting...\n");
		exit(0);

	} else {
		// Parent process

if (0) {
	// Unmap when done
	printf("Parent -- before munmapping mapped_region\n");
	sleep(1);
	ret = munmap(mapped_region, size);
	printf("Parent -- ret:%d\n", ret);
	assert(ret != -1);
}

		printf("Parent -- Wait for child to modify memory\n");
		sleep(1); // Wait for child to modify memory
		printf("Parent -- Reading memory:%c\n", mapped_region[0]);

		wait(NULL);
	}

	//getchar();
	printf("Parent -- before munmapping p1\n");
	sleep(5);
	ret = munmap(p1, sz);
	assert(ret != -1);

	//getchar();

if (1) {
	// Unmap when done
	printf("Parent -- before munmapping mapped_region\n");
	sleep(5);
	ret = munmap(mapped_region, size);
	printf("Parent -- ret:%d\n", ret);
	assert(ret != -1);
}

	printf("Parent -- before exiing...\n");
	sleep(3);
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
main(int argc, char *argv[])
{
	int ch;
	bool wflag;

	printf("Real Hello World\n");

	wflag = false;

	while ((ch = getopt(argc, argv, "w")) != -1) {
		switch (ch) {
		case 'w':
			wflag = true;
			break;
		default:
			printf("-----\n");
		}
	}
	argc -= optind;
	argv += optind;

	//mmap_test_anon();

	/* important -- make rhw_mem_map */
	//mmap_test_file_fork();

	//mmap_test_anon_fork();
	//mmap_test();
	//sysread_test();
	//rw_test();

	//getchar();

	file_mmap(wflag);
	return 0;
}
