#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

int main(){
    printf("PID %d\n", getpid());
    sleep(10);

    char *tmp[10000];
    printf("start malloc\n");

    for(size_t i = 0; i < 10000; i++) {
        tmp[i] = (char *)malloc(4096 * sizeof(char));
        memset(tmp[i], 'c', 4096 * sizeof(char));
        sleep(1);
    }

    printf("Sleep\n");
    sleep(5);

    // Deallocate the memory
    for(size_t i = 0; i < 10000; i++) {
        free(tmp[i]);
    }

}