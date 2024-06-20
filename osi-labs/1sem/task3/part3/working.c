#include <stdio.h>
#include <stdlib.h>
#include <stdint-gcc.h>
#include <errno.h>
#include <unistd.h>
#include "smart_buffer.h"
#include "stdbool.h"

#define PAGE_SIZE 0x1000

// maps file
//<address start>-<address end>  <mode>  <offset>   <major id:minor id>   <inode id>   <file path>  
// 559b8c418000-559b8c41a000      r--p    00000000          08:30               1708     /usr/bin/cat




struct SmartBuf* create_reversed_smart_buf(struct SmartBuf *in){
    struct SmartBuf* res = create_smart_buffer();
    for(int i = (int)in->length - 1; i >= 0; i--){
        push_char_to_buffer(res, in->buf[i]);
    }
    return res;
}


void print_lib_name(struct SmartBuf *lib_name);

void parse_lib_name(const char *line, ssize_t readCount, struct SmartBuf *lib_name) {
    struct SmartBuf *tmp = create_smart_buffer();
    for(int i = readCount - 2; i > 0; i--){
        if(line[i] == ' '){
            break;
        }

        push_char_to_buffer(tmp, line[i]);
    }

    struct SmartBuf *reversed = create_reversed_smart_buf(tmp);
    push_char_array_to_buffer(lib_name, reversed->buf);
    free_smart_buffer(tmp);
    free_smart_buffer(reversed);
}

static void print_page(uint64_t address, uint64_t data) {

    printf("0x%-16lx(0x%lx) : pfn %-16lx soft-dirty %ld file/shared %ld "
           "swapped %ld present %ld library",
           address,
           PAGE_SIZE * (data & 0x7fffffffffffff) + address,
           data & 0x7fffffffffffff,
           (data >> 55) & 1,
           (data >> 61) & 1,
           (data >> 62) & 1,
           (data >> 63) & 1
           );
}

void handle_virtual_range(int pagemap_file, uint64_t start_address,
                          uint64_t end_address, struct SmartBuf *lib_name) {

    for(uint64_t i = start_address; i < end_address; i += 0x1000) {
        uint64_t data;
        uint64_t index = (i / PAGE_SIZE) * sizeof(data);
        bool is_error = false;
        if(pread(pagemap_file, &data, sizeof(data), (long)index) != sizeof(data)) {
            if(errno){
                perror("pread");
            }
            is_error = true;
        }

        print_page(i, data);
        print_lib_name(lib_name);

        if(is_error){
            break;
        }


    }
}

void print_lib_name(struct SmartBuf *lib_name) {
    printf(" ");
    print_smart_buffer(lib_name);
    printf("\n");
}

void set_address_ranges(char *line, size_t length, struct SmartBuf *start, struct SmartBuf *stop){
    bool is_dash_found = false;
    for(size_t i = 0; i < length; i++){
        if(line[i] == ' '){
            break;
        }

        if(line[i] == '-'){
            is_dash_found = true;
            continue;
        }

        if(!is_dash_found){
            push_char_to_buffer(start, line[i]);
        }else{
            push_char_to_buffer(stop, line[i]);
        }


    }
}

void handle_error(FILE *maps_file, char *line, struct SmartBuf *start, struct SmartBuf *stop, struct SmartBuf *lib_name,
                  int res) {
    if(res != 1 ) {
        perror("Did not convert the value");
        free_smart_buffer(start);
        free_smart_buffer(stop);
        free_smart_buffer(lib_name);
        if (line)
            free(line);
        fclose(maps_file);
        exit(1);
    }
}

void parse_maps(const char *maps_file_name, const char *pagemap_file_name) {
    FILE *maps_file;
    char * line = NULL;
    size_t len = 0;
    ssize_t readCount;

    if((maps_file=fopen(maps_file_name, "r"))==NULL) {
        printf ("Cannot open file.\n");
        exit(1);
    }

    FILE* pagemap = fopen(pagemap_file_name, "r");
    if(pagemap < 0) {
        printf ("Cannot open pagemap file.\n");
        fclose(pagemap);
        return;
    }

    while ((readCount = getline(&line, &len, maps_file)) != -1) {
//        printf("Retrieved line of length %zu:\n", readCount);


        struct SmartBuf* start = create_smart_buffer();
        struct SmartBuf* stop = create_smart_buffer();
        struct SmartBuf* lib_name = create_smart_buffer();
        set_address_ranges(line, readCount, start, stop);
        uint64_t start_parsed_address, end_parsed_address;
        int res = sscanf(start->buf,"%lx",&start_parsed_address);
        handle_error(maps_file, line, start, stop, lib_name, res);
        res = sscanf(stop->buf,"%lx",&end_parsed_address);
        //printf("%lx %lx", start_parsed_address, end_parsed_address);
        parse_lib_name(line, readCount, lib_name);
        handle_error(maps_file, line, start, stop, lib_name, res);

    



        handle_virtual_range(fileno(pagemap), start_parsed_address, end_parsed_address, lib_name);
        free_smart_buffer(start);
        free_smart_buffer(stop);
        free_smart_buffer(lib_name);
    }

    if (line)
        free(line);
    fclose(maps_file);
}


void process_pid(pid_t pid) {
    char maps_file[BUFSIZ];
    char pagemap_file[BUFSIZ];
    snprintf(maps_file, sizeof(maps_file),
        "/proc/%lu/maps", (uint64_t)pid);
    snprintf(pagemap_file, sizeof(pagemap_file),
        "/proc/%lu/pagemap", (uint64_t)pid);

    parse_maps(maps_file, pagemap_file);
}

int main(int argc, char **argv){
    if(argc < 2) {
        printf("Usage: %s pid\n", argv[0]);
        return 1;
    }

    pid_t pid = strtoul(argv[1], NULL, 10);
    printf("=== Maps for pid %d\n", pid);
    process_pid(pid);

    return 0;
}