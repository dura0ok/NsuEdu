#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <libgen.h>
#include <sys/stat.h>
#include <unistd.h>
#include "smart_buffer.h"


struct SmartBuf* create_reversed_smart_buf(struct SmartBuf *in){
    struct SmartBuf* res = create_smart_buffer();
    for(int i = (int)in->length - 1; i >= 0; i--){
        push_char_to_buffer(res, in->buf[i]);
    }
    return res;
}

void updateResult(struct SmartBuf *tmp, struct SmartBuf *res) {
    struct SmartBuf* reversed = create_reversed_smart_buf(tmp);
    push_char_array_to_buffer(res, reversed->buf);
    push_char_to_buffer(res, '/');
    free_smart_buffer(reversed);
}

struct SmartBuf* reverse_names_in_path(struct SmartBuf *in){
    struct SmartBuf *tmp = create_smart_buffer();
    struct SmartBuf *res = create_smart_buffer();
    for(size_t i = 0; i < in->length; i++){
        if(in->buf[i] == '/'){
            updateResult(tmp, res);
            clear_smart_buffer(tmp);
        }else{
            push_char_to_buffer(tmp, in->buf[i]);
        }

    }
    if(tmp->length != 0){
        updateResult(tmp, res);
    }
    free_smart_buffer(tmp);
    return res;
}


int walker(struct SmartBuf *cur_path) {
    DIR *currentDirectory = opendir (cur_path->buf);
    if(currentDirectory == NULL){
        fprintf(stderr, "%s %s\n", "can`t open currentDirectory ", cur_path->buf);
        return 1;
    }

    struct SmartBuf* new_renaming_dir_path = reverse_names_in_path(cur_path);
    print_smart_buffer(new_renaming_dir_path);
    printf("\n");


    struct stat st = {0};

    if (stat(new_renaming_dir_path->buf, &st) != -1) {
        printf("directory already exists");
        free_smart_buffer(new_renaming_dir_path);
        closedir(currentDirectory);
        return 1;
    }

    mkdir(new_renaming_dir_path->buf, 0700);

    struct dirent* element;
    while (( element = readdir(currentDirectory)) != NULL ) {
        if (!strcmp(element->d_name, ".")) continue;
        if (!strcmp(element->d_name, "..")) continue;
        struct SmartBuf *new_path = create_smart_buffer();
        push_char_array_to_buffer(new_path, cur_path->buf);
        push_char_to_buffer(new_path, '/');
        push_char_array_to_buffer(new_path, element->d_name);

        if (element->d_type == DT_DIR){
            walker(new_path);

        }
        if(element->d_type == DT_REG){
            struct SmartBuf* tmp = create_smart_buffer();
            push_char_array_to_buffer(tmp, element->d_name);
            struct SmartBuf* reversed = create_reversed_smart_buf(tmp);

            struct SmartBuf *sourceBuf = create_smart_buf_from_raw_data(cur_path->buf);
            push_char_to_buffer(sourceBuf, '/');
            push_char_array_to_buffer(sourceBuf, element->d_name);

            struct SmartBuf *destBuf = create_smart_buf_from_raw_data(new_renaming_dir_path->buf);
            push_char_array_to_buffer(destBuf, reversed->buf);

            char buffer[BUFSIZ];
            FILE *source = fopen (sourceBuf->buf, "r");
            FILE *dest = fopen (destBuf->buf, "w");

            char c;
            fseek(source, -1, SEEK_END);

            while(1){
                c = (char)fgetc(source);
                //printf("asd %c\n", c);
                fputc(c, dest);
                if(fseek(source, -2L, SEEK_CUR) != 0){
                    break;
                }

                if(feof(source)){
                    break;
                }
            }


            fclose(dest);
            fclose(source);
            free_smart_buffer(tmp);
            free_smart_buffer(reversed);
            free_smart_buffer(destBuf);
            free_smart_buffer(sourceBuf);
        }
        free_smart_buffer(new_path);
    }

    free_smart_buffer(new_renaming_dir_path);
    closedir(currentDirectory);
    return 0;
}




int main(int argc, char **argv) {
    if(argc < 2){
        printf("choose catalog path");  
        return 1;
    }
    char *catalog_base = argv[1];
    struct SmartBuf* catalog_base_smart_buf = create_smart_buf_from_raw_data(catalog_base);
//    struct SmartBuf* res = reverse_names_in_path(catalog_base_smart_buf);
//    print_smart_buffer(res);
//    free_smart_buffer(res);
    int ret_value = walker(catalog_base_smart_buf);
    free_smart_buffer(catalog_base_smart_buf);
    return ret_value;
}

