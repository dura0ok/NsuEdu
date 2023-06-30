#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <limits.h>

void print_error() { printf("Error: %s\n", strerror(errno)); }


int main(int argc, char **argv){
    struct dirent *entry;
    DIR *dir = opendir(argv[1]);
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            char path[BUFSIZ];
            sprintf(path, "%s/%s", argv[1], entry->d_name);
            printf("%s\n", path);
            remove(path);
        }
    }
    if(rmdir(argv[1]) != 0){
        print_error();
    }
    return 0;
}