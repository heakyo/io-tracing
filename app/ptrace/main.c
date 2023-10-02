#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
	pid_t child;
	int ret, status;

	child = fork();

	if (!child) { /* child process */

		ret = ptrace(PT_TRACE_ME, 0, NULL, 0);
		if (ret) {
			perror("ptrace");
			exit(1);
		}

		//printf("This is child, will be traced...\n");
		execl("/bin/date", "date", NULL);

		exit(0);
	} else { /* parent process */

		wait(&status);
		//printf("1.status:0x%x\n", status);

		while (WIFSTOPPED(status)) {
			//printf("status:0x%x\n", WIFSTOPPED(status));

			ret = ptrace(PT_CONTINUE, child, (caddr_t)1, 0);
			if (ret) {
				perror("ptrace");
				exit(1);
			}


			wait(&status);
			//printf("2.status:0x%x\n", status);
		}
	}

	return 0;
}
