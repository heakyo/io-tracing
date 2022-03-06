#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define IOT_PRINT(fmt, ...)    \
	do {                   \
		if (v)         \
			printf(fmt, ## __VA_ARGS__); \
	} while(0)

#define BUFSZ (4096 * 1)

void usage()
{
	printf("Usage:\n\t"
		"main [OPTIONS] [<device>]\n"
		"\nOptions:\n"
		"[  --write, -w ] --- write data to the device\n"
		"[  --read,  -r ] --- read data from the device\n"
		"[  --pattern=<PTN>, -p <PTN> ]  --- data pattern\n"
		"[  --verbose, -v ] --- print more information\n"
		"[  --help, -h ] --- show help\n"
		"\nExamples:\n"
		"\t1.write data to the device dm-0 with pattern 0xb5\n"
		"\tmain -p a5 -w /dev/dm-0\n"
		);
	_exit(-1);
}

int main(int argc, char *argv[])
{
	char dev[32];
	int wr, rd, v;
	int fd, ret;
	unsigned char wrdata_pattern;
	unsigned char *wrbuf, *rdbuf;

	char ch;
	char *short_opts = "wrp:vh";
	struct option long_opts[] = {
		{"write", no_argument, NULL, 'w'},
		{"read", no_argument, NULL, 'r'},
		{"pattern", required_argument, NULL, 'p'},
		{"verbose", no_argument, NULL, 'v'},
		{"help", no_argument, NULL, 'h'},
		{NULL, 0, NULL, 0},
	};

	wr = rd = 0;
	v = 0;
	opterr = 0;

	wrdata_pattern = 0;

	while ((ch = getopt_long_only(argc, argv, short_opts, long_opts, NULL)) != -1) {
		switch (ch) {
		case 'w':
			wr = 1;
			break;
		case 'r':
			rd = 1;
			break;
		case 'p':
			wrdata_pattern = strtol(optarg, NULL, 16);
			break;
		case 'v':
			v = 1;
			break;
		case 'h':
			usage();
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

	IOT_PRINT("Start test.....\n");

	strcpy(dev, argv[optind]);
	fd = open(dev, O_RDWR | O_CREAT | O_SYNC | O_DIRECT, S_IRWXU | S_IRWXG | S_IRWXO);
	if (fd == -1) {
		perror("open failed.");
		return -1;
	}

	if (wr) {
		ret = posix_memalign((void **)&wrbuf, BUFSZ, BUFSZ);
		if (ret) {
			printf("posix_memalign failed! %s\n", strerror(ret));
			return -1;
		}
		memset(wrbuf, wrdata_pattern, BUFSZ);
		IOT_PRINT("wr:buf:%p pattern:0x%x\n", wrbuf, wrdata_pattern);

		ret = write(fd, wrbuf, BUFSZ);
		if (ret == -1) {
			perror("write failed.");
			return ret;
		}

		free(wrbuf);
	}

	lseek(fd, 0, SEEK_SET);

	if (rd) {
		ret = posix_memalign((void **)&rdbuf, BUFSZ, BUFSZ);
		if (ret) {
			printf("posix_memalign failed! %s\n", strerror(ret));
			return -1;
		}
		memset(rdbuf, 0x0, BUFSZ);

		ret = read(fd, rdbuf, BUFSZ);
		if (ret == -1) {
			perror("read failed.");
			return ret;
		}

		IOT_PRINT("rd:buf:%p 0x%x--0x%x\n", rdbuf, rdbuf[0], rdbuf[BUFSZ-1]);

		free(rdbuf);
	}

	close(fd);

	IOT_PRINT("End test.....\n");

	return 0;
}
