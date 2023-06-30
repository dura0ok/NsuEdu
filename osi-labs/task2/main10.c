#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <sys/syscall.h>

int main(int argc, char **argv){
    if(argc < 2){
        printf("Pass program path!");
        exit(1);
    }
    
    pid_t child;
    long orig_rax, rax;
    int status;
    struct user_regs_struct regs;

    child = fork();
    if (child == 0) {
        // Child process
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);

        execvp(argv[1], argv + 1);
        //asm volatile ("int3");
        
    }
    else {
        // Parent process
        wait(&status);
        
        while (WIFSTOPPED(status)) {
            // Get the system call number
            ptrace(PTRACE_GETREGS, child, NULL, &regs);
            orig_rax = regs.orig_rax;
            printf("System call %ld made by child\n", orig_rax);
            
            // Continue the child process
            ptrace(PTRACE_SYSCALL, child, NULL, NULL);
            wait(&status);
        }
    }
    
    return 0;
}