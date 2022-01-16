#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFSZ 64

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
	char rdbuf[BUFSZ], wrbuf[BUFSZ];

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

	strcpy(dev, argv[optind]);
	fd = open(dev, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	if (fd == -1) {
		perror("open failed.");
		return -1;
	}

	memset(rdbuf, 0x0, sizeof(rdbuf));
	memset(wrbuf, 0xa5, sizeof(wrbuf));

	if (wr) {
		ret = write(fd, wrbuf, sizeof(wrbuf));
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

	return 0;
}
