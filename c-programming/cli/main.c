#include <stdio.h>
#include "string.h"
#include "converters.h"
#include "stdbool.h"

bool check_exit_command(char input[BUFSIZ]){
    if(strcmp(input, "exit") == 0){
        printf("exiting");
        return true;
    }
    return false;
}

void print_error(){
    printf("error\n");
}



int main(int argc, char * argv[]) {
    printf("Hello!\n I can convert from\n (metre, miles, feet) to (metre, miles, feet)\n");
    bool is_interactive = argc > 1 && strcmp(argv[1], "-i") == 0;

    while(1) {
        char from[BUFSIZ];
        char to[BUFSIZ];


        printf("from: ");
        fgets(from, BUFSIZ, stdin);
        from[strcspn(from, "\n")] = 0;
        if(check_exit_command(from)) break;
        struct Converter *main_converter = find_to_main_converter(from);
        if (main_converter == NULL){
            print_error();
            if(!is_interactive) return 0;
            continue;
        }


        printf("to: ");
        fgets(to, BUFSIZ, stdin);
        to[strcspn(to, "\n")] = 0;
        if(check_exit_command(to)) break;
        struct Converter *from_main_converter = find_from_main_converter(to);
        if (from_main_converter == NULL) {
            print_error();
            if(!is_interactive) return 0;
            continue;
        }

        double value;
        char raw_value[BUFSIZ];
        printf("value: ");
        fgets(raw_value, BUFSIZ, stdin);
        if(sscanf(raw_value, "%lf", &value) != 1){
            print_error();
            if(!is_interactive) return 0;
            continue;
        }

        printf("Result: %f\n", from_main_converter->func(main_converter->func(value)));

    }
    return 0;
}
