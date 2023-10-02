#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Usage: %s <pid>\n", argv[0]);
        exit(1);
    }

    pid_t target_pid = atoi(argv[1]);
    int status;

    printf("Attaching to process %d...\n", target_pid);
    if (ptrace(PT_ATTACH, target_pid, NULL, 0) != 0) {
        perror("ptrace(PT_ATTACH)");
        exit(1);
    }

    // Wait for the target process to stop
    if (waitpid(target_pid, &status, 0) < 0) {
        perror("waitpid");
        exit(1);
    }

    printf("Loop......\n");
    while (1);

    printf("Process %d stopped. Detaching...\n", target_pid);

    if (ptrace(PT_DETACH, target_pid, NULL, 0) != 0) {
        perror("ptrace(PT_DETACH)");
        exit(1);
    }

    printf("Detached from process %d.\n", target_pid);

    return 0;
}
