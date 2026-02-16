#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define BUFSZ (16*1024)

bool run_pread, run_sysmap;

static int
read_file(void)
{
	unsigned char buf[BUFSZ], *p1, c;
	int fd;
	off_t offset;
	int ret;
        size_t length;
        struct stat sb;

	memset(buf, 0x0C, sizeof(buf));

	fd = open("ufs2demo_mntdir/myfirstfile", O_CREAT | O_RDWR | O_SYNC, 0777);
	if (fd == -1) {
		perror("Failed to open file");
		exit(-1);
	}

	ret = pwrite(fd, buf, sizeof(buf), 0);
	//ret = 0;
	assert(ret != -1);

	if (run_sysmap) {

		ret = fstat(fd, &sb);
		assert(ret != -1);
		length = sb.st_size;
		//printf("length:%zu\n", length);

		//printf("Memory mapping....\n");
		p1 = mmap(NULL, length, PROT_READ | PROT_WRITE,
			MAP_PRIVATE,
			fd, 0);
		assert(p1 != MAP_FAILED);

		/* Trigger page fault interrupt */
		c = p1[0];

		//printf("Writing....\n");
		//memset(p1, 0xb5, 4096);
		//msync(p1, length, MS_SYNC);

		//printf("Memory unmapping....\n");
		ret = munmap(p1, length);
		assert(ret != -1);

	} else if (run_pread) {
		offset = 0;
		ret = pread(fd, buf, sizeof(buf), offset);
		assert(ret != 1);

		//printf("0x%x-0x%x\n", buf[0], buf[BUFSZ - 1]);
	}

	close(fd);

	return 0;
}

void
usage(void)
{
	printf("usage:\t./main [-r] [-s] [-h]\n\t");
	printf("-r: Use pread syscall\n\t");
	printf("-s: Use sysmap syscall\n\t");
	printf("-h: Show help info\n");

	exit(-1);
}

int
main(int argc, char *argv[])
{
        int ch;
	run_pread = false;
	run_sysmap = false;

	if (argc == 1)
		usage();

        while ((ch = getopt(argc, argv, "rsh")) != -1) {
                switch (ch) {
                case 'r':
                        run_pread = true;
                        break;
                case 's':
                        run_sysmap = true;
                        break;
		case 'h':
			usage();
			break;
                default:
                        printf("-----\n");
			usage();
                }
        }
        argc -= optind;
        argv += optind;

	if (run_pread == true && run_sysmap == true)
		printf("pread and sysmap can not be all true\n");

	read_file();

	return 0;
}
