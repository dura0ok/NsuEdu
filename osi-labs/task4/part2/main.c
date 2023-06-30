#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv){
    printf("PID %d\n", getpid());
    sleep(2);
    execv(argv[0], argv);
    printf("Hello world");
}