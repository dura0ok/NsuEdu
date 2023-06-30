#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <limits.h>

void print_error() {
 fprintf(stderr, "%s\n", strerror(errno));
}


int is_directory(const char* fileName){
    struct stat path;
    if(stat(fileName, &path) != 0){
        print_error();
        exit(1);
    }

     return S_ISDIR(path.st_mode) != 0; 
}

int file_printer(char *file_name) {
    if(access(file_name, F_OK != 0)){
        fprintf(stderr, "%s", "File not found");
        return 1;
    }

    FILE *f = fopen (file_name, "r");
    if(f == 0){
        fprintf(stderr, "%s", "file read error");
        return 1;
    }

    char buffer[BUFSIZ] = {'\0'};

    while(1){
        size_t n = fread(buffer, sizeof(char), BUFSIZ, f);
        if(n == 0){
            break;
        }

        for(size_t i = 0; i < n; i++){
            printf("%c", buffer[i]);
        }
    }


    fclose(f);
    return 0;
}


// getops

int main(int argc, char **argv) {
    char* command = strdup(argv[0]);
    char* base_name = basename(command);
    printf("Entered command: %s\n", base_name);
    if(argc < 2){
        fprintf(stderr, "%s", "something went wrong");
        return 1;
    }

    if(strcmp(base_name, "make_dir") == 0){
        if(mkdir(argv[1], 0700) != 0){
            print_error();
            return 1;
        }
        return 0;
    }

    if(strcmp(base_name, "show_dir") == 0){
        DIR *currentDirectory = opendir (argv[1]);
        if(currentDirectory == NULL){
            fprintf(stderr, "%s %s\n", "can`t open currentDirectory ", argv[1]);
            return 1;
        }

        struct dirent* element;
        while (( element = readdir(currentDirectory)) != NULL ){
            if ( !strcmp( element->d_name, "."  )) continue;
            if ( !strcmp( element->d_name, ".." )) continue;
            printf("%s\n", element->d_name);
        }

        return 0;
    }

    if(strcmp(base_name, "rm_dir") == 0){
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

    if(strcmp(base_name, "create_file") == 0){
        if(access(argv[1], F_OK) == 0){
            fprintf(stderr, "%s", "file create error. Already exists");
            return 1;
        }

        FILE *f = fopen (argv[1], "w");
        if(f == 0){
            fprintf(stderr, "%s", "file create error");
            return 1;
        }

        fclose(f);
        return 0;
    }

    if(strcmp(base_name, "print_file") == 0){
        printf("Попытаюсь сделать вывод содержимого\n");
        return file_printer(argv[1]);
    }

    if(strcmp(base_name, "rm_file") == 0){
        if(access(argv[1], F_OK) != 0){
            fprintf(stderr, "%s", "File not found");
            return 1;
        }

        if(is_directory(argv[1])){
            fprintf(stderr, "%s", "This is directory. Error");
            return 1;
        }

        if(remove(argv[1]) != 0){
            fprintf(stderr, "%s", "Something went wrong when deleting files");
        }
        return 0;
    }

    if(strcmp(base_name, "create_symbolic_link") == 0){
        if(argc < 3){
            fprintf(stderr, "%s", "You need to pass args from and to!!!");
            return 1;
        }

        //printf("%d", symlink(argv[1], argv[2]));
        if(symlink(argv[1], argv[2]) == -1){
            fprintf(stderr, "%s", "error when trying create symbolic_link\n");
            print_error();
            unlink(argv[2]);
            return 1;
        }

        return 0;
    }

    if(strcmp(base_name, "read_link") == 0){
        char buf[BUFSIZ];
        int n_bytes = (int)readlink(argv[1], buf, BUFSIZ);
        if(n_bytes < 0){
            fprintf(stderr, "%s", "read link error\n");
            print_error();
            return 1;
        }


        if (n_bytes == BUFSIZ){
            printf("(Returned buffer may have been truncated)\n");
        }

        printf("'%s' points to '%.*s'\n", argv[1], (int) n_bytes, buf);

        return 0;
    }

    if(strcmp(base_name, "read_link_content") == 0){

        ssize_t link_size;
        struct stat sb;

        if (lstat(argv[1], &sb) == -1) {
            perror("lstat");
            exit(EXIT_FAILURE);
        }

        link_size = sb.st_size;
        char *buf = (char*)malloc(sizeof(char) * (link_size + 1));

       
        int n_bytes = (int)readlink(argv[1], buf, link_size);
        buf[n_bytes+1] = '\0';

        if(n_bytes < 0){
            fprintf(stderr, "%s", "read link error\n");
            print_error();
            free(buf);
            return 1;
        }

        file_printer(buf);
        free(buf);
        return 0;
    }

    if(strcmp(base_name, "delete_symbolic_link") == 0){
        if(unlink(argv[1]) != 0){
            fprintf(stderr, "%s", "error when trying unlink symbolic_link\n");
            print_error();
            return 1;
        }
        return 0;
    }

    if(strcmp(base_name, "create_hard_link") == 0){
        if(argc < 3){
            fprintf(stderr, "%s", "You need to pass args from and to!!!");
            return 1;
        }

        //printf("%d", symlink(argv[1], argv[2]));
        if(link(argv[1], argv[2]) == -1){
            fprintf(stderr, "%s", "error when trying create hard link\n");
            print_error();
            unlink(argv[2]);
            return 1;
        }
        return 0;
    }

    if(strcmp(base_name, "delete_hard_link") == 0) {
        if(unlink(argv[1]) != 0){
            fprintf(stderr, "%s", "error when trying unlink hardlink\n");
            print_error();
            return 1;
        }
        return 0;
    }

    if(strcmp(base_name, "print_file_info") == 0){
        struct stat fileStat;
        if(lstat(argv[1], &fileStat) < 0){
            print_error();
            return 1;
        }


        printf("Information for %s\n", argv[1]);
        printf("Number of hard Links: \t%ld\n", fileStat.st_nlink);

        printf("File Permissions: \t");
        printf( (S_ISDIR(fileStat.st_mode)) ? "d" : "n/d");
        printf( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
        printf( (fileStat.st_mode & S_IWUSR) ? "w" : "-");
        printf( (fileStat.st_mode & S_IXUSR) ? "x" : "-");
        printf( (fileStat.st_mode & S_IRGRP) ? "r" : "-");
        printf( (fileStat.st_mode & S_IWGRP) ? "w" : "-");
        printf( (fileStat.st_mode & S_IXGRP) ? "x" : "-");
        printf( (fileStat.st_mode & S_IROTH) ? "r" : "-");
        printf( (fileStat.st_mode & S_IWOTH) ? "w" : "-");
        printf( (fileStat.st_mode & S_IXOTH) ? "x" : "-");
        printf("\n\n");

        printf("The file %s a symbolic link\n", (S_ISLNK(fileStat.st_mode)) ? "is" : "is not");

        return 0;
    }

    if(strcmp(base_name, "change_file_access") == 0){
        if(argc < 3){
            fprintf(stderr, "%s", "You need choose file and rights!");
            return 1;
        }

        char * end_ptr;
        long mode = strtol(argv[2], &end_ptr, 10);

        if (end_ptr == argv[2]) {
            fprintf(stderr, "%s: not a decimal number\n", argv[2]);
            return 1;
        } else if ('\0' != *end_ptr) {
            fprintf(stderr, "%s: extra characters at end of input: %s\n", argv[2], end_ptr);
            return 1;
        } else if ((LONG_MIN == mode || LONG_MAX == mode) && ERANGE == errno) {
            fprintf(stderr, "%s out of range of type long\n", argv[2]);
            return 1;
        } else if (mode > INT_MAX) {
            fprintf(stderr, "%ld greater than INT_MAX\n", mode);
            return 1;
        } else if (mode < INT_MIN) {
            fprintf(stderr, "%ld less than INT_MIN\n", mode);
            return 1;
        }

        if(chmod(argv[1], (int)mode) != 0) {
            print_error();
            return 1;
        }

        return 0;
    }




    printf("Command not found :C");
    free(command);
    return 0;
}