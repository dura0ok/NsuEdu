#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int b = 3;

int main(){
    int a = 5;
    int status;
    printf("pid %d\n", getpid());
    // sleep(10);
    int pid = fork();
    if (pid == -1) {
        printf("Failed to fork process. \n");
         exit(EXIT_FAILURE);
    }

    if(pid == 0){
        printf("pid %d parent pid %d\n", getpid(), getppid());
        printf("child local 0x%p %d\n", &a, a);
        printf("child global 0x%p %d\n", &b, b);
        a = 10;
        b = 20;

        printf("child local 0x%p %d\n", &a, a);
        printf("child global 0x%p %d\n", &b, b);
        sleep(10);
        exit(5);
    }else{
          a = 11;
          b = 21;
          printf("parent local 0x%p %d\n", &a, a);
          printf("parent global 0x%p %d\n", &b, b);
          kill(pid, SIGINT);
           wait(&status);
            if (WIFEXITED(status)) {    
                printf("Child process terminated normally with exit code %d\n", WEXITSTATUS(status));
            }
            else if (WIFSIGNALED(status)) {
                printf("Child process terminated by signal %d\n", WTERMSIG(status));
                printf("Child process terminated normally with exit code %d\n", WEXITSTATUS(status));
            }
            else if (WIFSTOPPED(status)) {
                printf("Child process stopped by signal %d\n", WSTOPSIG(status));
            }
            else {
                printf("Unknown termination reason\n");
            }
    }
}