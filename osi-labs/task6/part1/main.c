#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sched.h>
#include <string.h>
#include <sys/wait.h>

#define PAGE_SIZE (4096)
#define REGION_SIZE (PAGE_SIZE)

volatile unsigned int *region;

void writer() {
    unsigned int i = 0;
    while (1) {
        region[i % REGION_SIZE / sizeof(unsigned int)] = i;
        i++;
    }
}

void reader() {
    unsigned int i = 0;
    while (1) {
        if (region[i % REGION_SIZE / sizeof(unsigned int)] != i) {
            printf("Sequence failure at index %u\n", i);
        }
        i++;
    }
}

int main() {
    region = mmap(NULL, REGION_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (region == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        writer();
    } else {
        reader();
    }

    return 0;
}
