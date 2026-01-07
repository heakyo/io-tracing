#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

int main(int argc, char *argv[])
{
	int fd;
	unsigned wrbuf[8192];
	int ret;

	fd = open("../mount/ufsimg/myfile", O_CREAT | O_RDWR | O_SYNC, 0777);
	assert(fd != -1);

	memset(wrbuf, 0xb5, sizeof(wrbuf));
	ret = write(fd, wrbuf, sizeof(wrbuf));
	assert(ret != -1);

	printf("ret:%d\n", ret);
	close(fd);

	return 0;
}
