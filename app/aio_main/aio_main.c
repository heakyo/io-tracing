#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libaio.h>

#define IOT_PRINT(fmt, ...)    \
	do {                   \
		if (v)         \
			printf(fmt, ## __VA_ARGS__); \
	} while(0)

#define BUFSZ (4096)

void usage()
{
	printf("Usage:\n\t"
		"main [OPTIONS] [<device>]\n"
		"\nOptions:\n"
		"[  --write, -w ] --- write data to the device\n"
		"[  --read,  -r ] --- read data from the device\n"
		"[  --pattern=<PTN>, -p <PTN> ]  --- data pattern\n"
		"[  --offset=<OFT>, -o <OFT> ]  --- offset\n"
		"[  --size=<SZ>, -s <SZ> ]  --- size in byte\n"
		"[  --verbose, -v ] --- print more information\n"
		"[  --help, -h ] --- show help\n"
		"\nExamples:\n"
		"\t1.write data to the device dm-0 with pattern 0xb5\n"
		"\tmain -p a5 -w /dev/dm-0\n"
		);
	_exit(-1);
}

#define AIO_IOCNT 10

int main(int argc, char *argv[])
{
	char dev[32];
	int wr, rd, v, aio;
	int fd, ret;
	unsigned char wrdata_pattern;
	unsigned char *wrbuf, *rdbuf;
	int offset, size;
	int i;

	io_context_t ctx;
	struct iocb io[AIO_IOCNT], *p[AIO_IOCNT];
	struct io_event e[1];
	unsigned nr_events = 1;

	char ch;
	char *short_opts = "wrp:o:s:avh";
	struct option long_opts[] = {
		{"write", no_argument, NULL, 'w'},
		{"read", no_argument, NULL, 'r'},
		{"pattern", required_argument, NULL, 'p'},
		{"offset", required_argument, NULL, 'o'},
		{"size", required_argument, NULL, 's'},
		{"aio", no_argument, NULL, 'a'},
		{"verbose", no_argument, NULL, 'v'},
		{"help", no_argument, NULL, 'h'},
		{NULL, 0, NULL, 0},
	};

	wr = rd = 0;
	v = 0;
	opterr = 0;

	wrdata_pattern = 0;
	offset = 0;
	size = BUFSZ;

	aio = 0;

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
		case 'o':
			offset = strtol(optarg, NULL, 10);
			break;
		case 's':
			size = strtol(optarg, NULL, 10);
			break;
		case 'a':
			aio = 1;
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
	IOT_PRINT("rd:%d wr:%d, oft:%d size:%d pattern:0x%x\n",
		rd, wr, offset, size, wrdata_pattern);

	strcpy(dev, argv[optind]);
	fd = open(dev, O_RDWR | O_CREAT | O_SYNC | O_DIRECT, S_IRWXU | S_IRWXG | S_IRWXO);
	if (fd == -1) {
		perror("open failed.");
		return -1;
	}

	if (aio) {
		printf("Using AIO to do IO test\n");

		memset(&ctx, 0x0, sizeof(io_context_t));
		ret = io_setup(nr_events, &ctx);
		if (ret != 0) {
			perror("io setup failed.");
			return ret;
		}

		for (i = 0; i < AIO_IOCNT; i++) {
			p[i] = &io[i];
		}
	}

	lseek(fd, offset, SEEK_SET);

	if (wr) {
		ret = posix_memalign((void **)&wrbuf, BUFSZ, size);
		if (ret) {
			printf("posix_memalign failed! %s\n", strerror(ret));
			return -1;
		}
		memset(wrbuf, wrdata_pattern, size);
		IOT_PRINT("wr:buf:%p pattern:0x%x\n", wrbuf, wrdata_pattern);

		if (aio) {
			io_prep_pwrite(&io[0], fd, wrbuf, size, offset);

			ret = io_submit(ctx, 1, p);
			if (ret < 0) {
				perror("io submit failed.");
				io_destroy(ctx);
				return ret;
			}

			while (1) {
				ret = io_getevents(ctx, 1, 1, e, NULL);
				if (ret < 0) {
					perror("io  failed.");
					io_destroy(ctx);
					return ret;
				}

				if (ret == 1) {
					printf("status: %ld, size: %ld\n", e[0].res2, e[0].res);
					break;
				}
			}

		} else {

			ret = write(fd, wrbuf, size);
			if (ret == -1) {
				perror("write failed.");
				return ret;
			}

		}

		free(wrbuf);
	}

	if (rd) {
		ret = posix_memalign((void **)&rdbuf, BUFSZ, size);
		if (ret) {
			printf("posix_memalign failed! %s\n", strerror(ret));
			return -1;
		}
		memset(rdbuf, 0x0, size);

		if (aio) {
			io_prep_pread(&io[0], fd, rdbuf, size, offset);

			ret = io_submit(ctx, 1, p);
			if (ret != 1) {
				perror("io submit failed.");
				io_destroy(ctx);
				return ret;
			}

			while (1) {
				ret = io_getevents(ctx, 1, AIO_IOCNT, e, NULL);
				if (ret < 0) {
					perror("io  failed.");
					io_destroy(ctx);
					return ret;
				}

				if (ret == 1) {
					printf("status: %ld, size: %ld\n", e[0].res2, e[0].res);
					break;
				}
			}

		} else {
			ret = read(fd, rdbuf, size);
			if (ret == -1) {
				perror("read failed.");
				return ret;
			}
		}

		IOT_PRINT("rd:buf:%p 0x%x--0x%x\n", rdbuf, rdbuf[0], rdbuf[size-1]);

		free(rdbuf);
	}

	io_destroy(ctx);

	close(fd);

	IOT_PRINT("End test.....\n");

	return 0;
}
