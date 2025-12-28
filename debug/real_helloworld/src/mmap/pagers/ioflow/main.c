#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

int main(int argc, char *argv[])
{
	int fd;

	fd = open("../mount/ufsimg/myfile", O_CREAT | O_RDWR, 0777);
	assert(fd != -1);

	close(fd);

	return 0;
}
