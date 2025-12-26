#define _GNU_SOURCE
#include <libaio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define BUF_SIZE 4096

int main(void)
{
	io_context_t ctx = 0;
	struct iocb cb;
	struct iocb *cbs[1];
	struct io_event events[1];

	/* 1. Setup AIO context */
	if (io_setup(1, &ctx) < 0) {
		perror("io_setup");
		return 1;
	}

	/* 2. Open file (must be O_DIRECT for real async behavior) */
	int fd = open("testfile", O_RDONLY | O_DIRECT);
	if (fd < 0) {
		perror("open");
		io_destroy(ctx);
		return 1;
	}

	/* 3. Allocate aligned buffer */
	void *buf;
	if (posix_memalign(&buf, 4096, BUF_SIZE)) {
		perror("posix_memalign");
		close(fd);
		io_destroy(ctx);
		return 1;
	}

	/* 4. Prepare read request */
	memset(&cb, 0, sizeof(cb));
	io_prep_pread(&cb, fd, buf, BUF_SIZE, 0);
	cbs[0] = &cb;

	/* 5. Submit AIO request */
	if (io_submit(ctx, 1, cbs) < 0) {
		perror("io_submit");
		free(buf);
		close(fd);
		io_destroy(ctx);
		return 1;
	}

	/* 6. Wait for completion */
	int ret = io_getevents(ctx, 1, 1, events, NULL);
	if (ret < 0) {
		perror("io_getevents");
	} else {
		printf("Read %ld bytes\n", events[0].res);
	}

	/* 7. Cleanup */
	free(buf);
	close(fd);
	io_destroy(ctx);
	return 0;
}
