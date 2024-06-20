#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid;

    // Fork the process
    pid = fork();

    if (pid == -1) {
        printf("Error: Failed to fork process\n");
        return 1;
    } else if (pid == 0) {
        // Child process
        exit(0);
    } else {
    	printf("Child pid %d\n", pid);
        // Parent process
        // check process status
        sleep(10);
        // Get child process status and print it
        int status;
        waitpid(pid, &status, 0);
        printf("Child process status: %d\n", status);
    }

    return 0;
}