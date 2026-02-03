#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#define WRBUFSZ (8*1024)

int main(int argc, char *argv[])
{
	int fd;
	unsigned char wrbuf[WRBUFSZ];
	int ret;

	fd = open("ufs2demo_mntdir/myfirstfile", O_CREAT | O_RDWR | O_SYNC, 0777);
	assert(fd != -1);

	memset(wrbuf, 0xb5, sizeof(wrbuf));
	ret = write(fd, wrbuf, sizeof(wrbuf));
	assert(ret != -1);

	//printf("ret:%d\n", ret);
	close(fd);

	return 0;
}
