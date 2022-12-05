#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>


#define FILE_NAME "data"
#define SIZE (4*1024*1024)


int main()
{
	int fd;
	char *p;
	int ret;

	fd = open(FILE_NAME, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
	if (fd < 0)
	        return -1;

	/* Set size of this file */
	ret = ftruncate(fd, SIZE);
	if (ret < 0)
	        return -1;

	/* The current offset is 0, so we don't need to reset the offset. */
	/* lseek(fd, 0, SEEK_CUR); */

	/* Mmap virtual memory */
	p = mmap(0, SIZE, PROT_READ|PROT_WRITE, MAP_FILE|MAP_SHARED, fd, 0);
	if (!p)
	        return -1;

	/* Alloc physical memory */
	memset(p, 1, SIZE);

	/* Lock these memory to prevent from being reclaimed */
	mlock(p, SIZE);

	printf("sleep...\n");
	getchar();

	/*
	 * Unmap the memory.
	 * Actually the kernel will unmap it automatically after the
	 * process exits, whatever we call munamp() specifically or not.
	 */
	munmap(p, SIZE);

	return 0;
}
