#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sched.h>
#include <string.h>
#include <sys/wait.h>

#define STACK_SIZE (1024 * 1024)
char *stack;

int second_function(char *str, int depth) {
    printf("%sn", str);
    if (depth < 2) {
        char new_str[20] = "hello world";
        return second_function(new_str, depth + 1);
    } else {
        
        return 0;
    }
}

int first_function(void *arg) {
    char *str = "hello world";
    return second_function(str, 1);
}

int main() {
    int fd = open("temp_file", O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
    if (fd == -1) {
        perror("Could not open file");
        exit(EXIT_FAILURE);
    }
    stack = mmap(NULL, STACK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, fd, 0);
    if (stack == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    pid_t pid = clone(first_function, stack + STACK_SIZE, CLONE_VM | CLONE_FILES | CLONE_FS | CLONE_SIGHAND | SIGCHLD | CLONE_PARENT_SETTID, NULL);
    if (pid == -1) {
        perror("clone");
        exit(EXIT_FAILURE);
    } 


    if(pid > 0){
         int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }
    }
   

    return 0;
}