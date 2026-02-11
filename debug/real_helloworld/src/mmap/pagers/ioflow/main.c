#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#define BUFSZ (8*1024)

int main(int argc, char *argv[])
{
	int fd;
	unsigned char buf[BUFSZ];
	int ret;

	fd = open("ufs2demo_mntdir/myfirstfile", O_CREAT | O_RDWR | O_SYNC, 0777);
	assert(fd != -1);

#if 0
	memset(buf, 0xb5, sizeof(buf));
	ret = write(fd, buf, sizeof(buf));
	assert(ret != -1);
#else
	memset(buf, 0x0, sizeof(buf));
	ret = read(fd, buf, sizeof(buf));
	assert(ret != -1);
#endif

	//printf("ret:%d\n", ret);
	close(fd);

	return 0;
}
