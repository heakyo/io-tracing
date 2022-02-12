#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFSZ 4096

void usage()
{
	printf("Usage:\n\t"
		"main [options] [<device>]\n"
		"Options:\n"
		" -w, --write\t write the dev\n"
		" -r, --read\t read the dev\n"
		);
	_exit(-1);
}

int main(int argc, char *argv[])
{
	char dev[32];
	int wr, rd;
	int fd, ret;
	char rdbuf[BUFSZ];
	char *aligned_wrbuf;

	char ch;
	char *short_opts = "wr";
	struct option long_opts[] = {
		{"write", no_argument, NULL, 'w'},
		{"read", no_argument, NULL, 'r'},
		{NULL, 0, NULL, 0},
	};

	wr = rd = 0;
	opterr = 0;

	while ((ch = getopt_long_only(argc, argv, short_opts, long_opts, NULL)) != -1) {
		switch (ch) {
		case 'w':
			wr = 1;
			break;
		case 'r':
			rd = 1;
			break;
		default:
			printf("Unknown option: %c\n", ch);
			usage();
		}
	}

	if (!argv[optind]) {
		printf("Need a device\n");
		usage();
	}

	ret = posix_memalign((void **)&aligned_wrbuf, BUFSZ, BUFSZ);
	if (ret) {
		printf("posix_memalign failed! %s\n", strerror(ret));
		return -1;
	}

	strcpy(dev, argv[optind]);
	fd = open(dev, O_RDWR | O_CREAT | O_SYNC | O_DIRECT, S_IRWXU | S_IRWXG | S_IRWXO);
	if (fd == -1) {
		perror("open failed.");
		return -1;
	}

	memset(rdbuf, 0x0, sizeof(rdbuf));
	memset(aligned_wrbuf, 0xa5, BUFSZ);

	if (wr) {
		ret = write(fd, aligned_wrbuf, BUFSZ);
		if (ret == -1) {
			perror("write failed.");
			return ret;
		}
	}

	if (rd) {
		ret = read(fd, rdbuf, sizeof(rdbuf));
		if (ret == -1) {
			perror("read failed.");
			return ret;
		}
	}

	close(fd);

	free(aligned_wrbuf);

	return 0;
}
