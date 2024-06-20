#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <sys/syscall.h>

int main(){
    pid_t child;
    long orig_rax, rax;
    int status;
    struct user_regs_struct regs;

    child = fork();
    if (child == 0) {
        // Child process
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execl("/bin/ls", "ls", NULL);
        // write(1, "Hi1\n", 4);
        // write(1, "Hi2\n", 4);
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