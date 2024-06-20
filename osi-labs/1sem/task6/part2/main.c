#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int pipe_fd[2], n;
    unsigned int buf;

    /* Create pipe */
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    /* Fork process */
    pid_t pid;
    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    /* Child process writes integers to pipe */
    if (pid == 0) {
        int i = 0;
        while (1) {
            buf = i;
            if (write(pipe_fd[1], &buf, sizeof(unsigned int)) == -1) {
                perror("write");
                exit(EXIT_FAILURE);
            }
            i++;
            usleep(1); // delay to slow down writing
        }
    }

    /* Parent process reads integers from pipe and checks for sequence failures */
    if (pid > 0) {
        int i = 0;
        while (1) {
            if ((n = read(pipe_fd[0], &buf, sizeof(unsigned int))) == -1) {
                perror("read");
                exit(EXIT_FAILURE);
            }
            printf("%d\n", n);
            if (n == 0) {
                break; // end of file
            }
            if (buf != i) {
                printf("Sequence failure at index %d: expected %d, actual %u\n", i, i, buf);
            }
            i++;
        }
    }

    exit(EXIT_SUCCESS);
}