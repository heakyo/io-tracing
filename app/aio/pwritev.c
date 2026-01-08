#define _GNU_SOURCE
#include <libaio.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>

#define PAGE_COUNT 64

struct iorec {
	io_context_t ctx;
	struct iocb *iovec;
	struct iovec iov[PAGE_COUNT];
};

int main(int argc, char *argv[])
{
	struct io_event events[1];
	struct iorec entry;

	memset(&entry, 0x0, sizeof(entry));

	entry.iovec = aligned_alloc(64, sizeof(struct iocb));
	if (!entry.iovec)
		assert(0);

	/* Create AIO context */
	if (io_setup(128, &entry.ctx) < 0) {
		perror("io_setup");
		return 1;
	}

	/* Open file */
	int fd = open("testfile.txt", O_CREAT | O_WRONLY | O_DIRECT, 0644);
	if (fd < 0) {
		perror("open");
		goto out_ctx;
	}

	/* Allocate aligned buffers (required for O_DIRECT) */
	char *buf1, *buf2;
	posix_memalign((void **)&buf1, 4096, 4096);
	posix_memalign((void **)&buf2, 4096, 4096);

	strcpy(buf1, "Hello ");
	strcpy(buf2, "libaio pwritev\n");

	entry.iov[0].iov_base = buf1;
	entry.iov[0].iov_len  = 4096;
	entry.iov[1].iov_base = buf2;
	entry.iov[1].iov_len  = 4096;

	/* Prepare pwritev request */
	io_prep_pwritev(entry.iovec, fd, entry.iov, 2, 0);
	entry.iovec->data = (void *)&entry;

	/* Submit request */
	int ret = io_submit(entry.ctx, 1, &entry.iovec);
	if (ret != 1) {
		perror("io_submit");
		goto out_fd;
	}

	/* Wait for completion */
	ret = io_getevents(entry.ctx, 1, 1, events, NULL);
	if (ret < 1) {
		perror("io_getevents");
		goto out_fd;
	}

	/* Check result */
	if (events[0].res < 0) {
		fprintf(stderr, "AIO write failed: %s\n",
			strerror(-events[0].res));
	} else {
		printf("AIO write completed: %ld bytes\n",
		       events[0].res);
	}

out_fd:
	close(fd);
	free(buf1);
	free(buf2);

out_ctx:
	io_destroy(entry.ctx);
	free(entry.iovec);

	return 0;
}
