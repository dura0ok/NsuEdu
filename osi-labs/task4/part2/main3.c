#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <signal.h>

void attach_pages() {
    printf("alert attach pages 10 sec\n");
    sleep(10);
    int page_size = getpagesize();
    int num_pages = 10;
    size_t size = page_size * num_pages;
    void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    printf("Successfully attached %d pages (%ld bytes) to the process.\n", num_pages, size);

    sleep(10);
    printf("Set protection flags to prohibit reading.\n");
    // Set the protection flags to prohibit reading
    int ret = mprotect(ptr, size, PROT_NONE);
    if (ret == -1) {
        perror("mprotect");
        exit(1);
    }

    printf("MUNMAP after 10 sec\n");
    sleep(10);

    // // Try to read from the memory region
    // printf("Trying to read from the memory region...\n");
    // int *data = (int *)ptr;
    // for (int i = 0; i < num_pages * page_size / sizeof(int); i++) {
    //     printf("%dn", data[i]);
    // }

     // Detach pages 4 to 6 in the region
    if (munmap(ptr + 4 * page_size, 3 * page_size) == -1) {
        printf("munmap failed\n");
        exit(1);
    }


    printf("Detached pages 4 to 6 in the memory regionn");
}

void attach_pages2() {
    int page_size = getpagesize();
    int num_pages = 10;
    size_t size = page_size * num_pages;
    void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    printf("Successfully attached %d pages (%ld bytes) to the process.\n", num_pages, size);

    // Set the protection flags to prohibit writing
    int ret = mprotect(ptr, size, PROT_READ);
    if (ret == -1) {
        perror("mprotect");
        exit(1);
    }
    printf("Set protection flags to prohibit writing.\n");

    // Try to write to the memory region
    printf("Trying to write to the memory region...\n");
    int *data = (int *)ptr;
    for (int i = 0; i < num_pages * page_size / sizeof(int); i++) {
        data[i] = i;
    }
}


void sigsegv_handler(int sig) {
    printf("HANDLED! Caught segmentation fault signal (SIGSEGV)\n");
    sleep(2);
}

int main(){
     signal(SIGSEGV, sigsegv_handler) ;
     //signal(SIGSEGV, SIG_IGN);
     printf("PID %d\n", getpid());

    printf("Sleep 10 sec\n");
    sleep(10);
    printf("Start attach\n");
    attach_pages();
    //attach_pages2();

    
    
}