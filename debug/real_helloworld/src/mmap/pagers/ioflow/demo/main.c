#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#define WRBUFLEN (32*1024)
#define FGLEN 8192

int main(int argc, char *argv[])
{
	int fd;
	unsigned wrbuf[WRBUFLEN];
	int ret, i;

	fd = open("../ufs2demo_mntdir/myfirstfile", O_CREAT | O_RDWR | O_SYNC, 0777);
	assert(fd != -1);

	for (i = 0; i < (WRBUFLEN/FGLEN); i++) {

		printf("value: 0x%x\n", 0x10 + i);
		memset(wrbuf, 0x10 + i, FGLEN);
		ret = write(fd, wrbuf, FGLEN);
		assert(ret != -1);
		printf("ret:%d\n", ret);
	}

	close(fd);

	return 0;
}
