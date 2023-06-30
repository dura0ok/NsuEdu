#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
   printf("Parent process %d\n", getpid());
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        printf("Child process\n");
        exit(EXIT_SUCCESS);
    } else {
        sleep(10); 
        sleep(2);
        printf("Parent process zombie now\n");
        exit(EXIT_SUCCESS);
    }
}