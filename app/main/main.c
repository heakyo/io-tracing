#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include <libaio.h>
#include <assert.h>

#define IOT_PRINT(fmt, ...)    \
	do {                   \
		if (debug)         \
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
		"[  --aio, -a ] --- use aio to read and write\n"
		"[  --verified, -v ] --- verify read/write data\n"
		"[  --debug, -d ] --- print more debug information\n"
		"[  --help, -h ] --- show help\n"
		"\nExamples:\n"
		"1.write data to the device dm-0 with pattern 0xb5\n"
		"\tmain -p b5 -w /dev/dm-0\n"
		"2.write data to the file with pattern 0xa5 in aio mode\n"
		"\tmain -p a5 -aw data\n"
		);
	_exit(-1);
}

#define AIO_IOCNT 10

int main(int argc, char *argv[])
{
	char dev[32];
	int wr, rd, debug, aio, verify;
	int fd, ret;
	unsigned char wrdata_pattern;
	unsigned char *wrbuf, *rdbuf;
	int offset, size;
	int i;

#if 0
	io_context_t ctx;
	struct iocb io[AIO_IOCNT], *p[AIO_IOCNT];
	struct io_event e[1];
	unsigned nr_events = 1;
#endif

	char ch;
	char *short_opts = "wrp:o:s:avdh";
	struct option long_opts[] = {
		{"write", no_argument, NULL, 'w'},
		{"read", no_argument, NULL, 'r'},
		{"pattern", required_argument, NULL, 'p'},
		{"offset", required_argument, NULL, 'o'},
		{"size", required_argument, NULL, 's'},
		{"aio", no_argument, NULL, 'a'},
		{"verify", no_argument, NULL, 'v'},
		{"debug", no_argument, NULL, 'd'},
		{"help", no_argument, NULL, 'h'},
		{NULL, 0, NULL, 0},
	};

	wr = rd = 0;
	debug = 0;
	opterr = 0;

	wrdata_pattern = 0;
	offset = 0;
	size = BUFSZ;

	aio = 0;

	ret = 0;

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
			verify = 1;
			break;
		case 'd':
			debug = 1;
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

#if 0
	if (aio) {
		IOT_PRINT("Using AIO to do IO test\n");

		memset(&ctx, 0x0, sizeof(io_context_t));
		ret = io_setup(nr_events, &ctx);
		if (ret) {
			perror("io setup failed.");
			return ret;
		}

		for (i = 0; i < AIO_IOCNT; i++)
			p[i] = &io[i];
	}
#endif

	strcpy(dev, argv[optind]);
	fd = open(dev, O_RDWR | O_CREAT | O_SYNC | O_DIRECT, S_IRWXU | S_IRWXG | S_IRWXO);
	assert(fd != -1);

	if (wr) {
		ret = posix_memalign((void **)&wrbuf, BUFSZ, size);
		assert(!ret);
		memset(wrbuf, wrdata_pattern, size);

		IOT_PRINT("wr:buf:%p pattern:0x%x\n", wrbuf, wrdata_pattern);

		if (aio) {
			//io_prep_pwrite(&io[0], fd, wrbuf, size, offset);
		} else {
			ret = pwrite(fd, wrbuf, size, offset);
			assert(ret != -1);
		}
	}

	if (rd) {
		ret = posix_memalign((void **)&rdbuf, BUFSZ, size);
		assert(!ret);
		memset(rdbuf, 0x0, size);

		if (aio) {
			//io_prep_pread(&io[0], fd, rdbuf, size, offset);
		} else {
			ret = pread(fd, rdbuf, size, offset);
			assert(ret != -1);
		}
	}

#if 0
	if (aio) {
		ret = io_submit(ctx, 1, p);
		assert(ret == 1);

		while (1) {
			ret = io_getevents(ctx, 1, AIO_IOCNT, e, NULL);
			assert(ret >= 0);

			if (ret == 1) {
				printf("status: %ld, size: %ld\n", e[0].res2, e[0].res);
				break;
			}
		}
	}
#endif

	if (verify) {
		printf("Verify: TBD\n");
	}

	if (wr)
		free(wrbuf);

	if (rd) {
		IOT_PRINT("rd:buf:%p 0x%x--0x%x\n", rdbuf, rdbuf[0], rdbuf[size-1]);
		free(rdbuf);
	}

#if 0
	if (aio)
		io_destroy(ctx);
#endif

	close(fd);

	IOT_PRINT("End test.....\n");

	return ret;
}
