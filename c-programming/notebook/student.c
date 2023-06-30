#include "student.h"
#include "malloc.h"
#include <sqlite3.h>
#include "string.h"
#include "stdbool.h"

void destroy_student(struct Student *destroy_user){
    if(destroy_user == NULL) return;
    if(destroy_user->first_name != NULL){
        free(destroy_user->first_name);
    }

    if(destroy_user->last_name != NULL){
        free(destroy_user->last_name);
    }

    if(destroy_user->group_identifier != NULL){
        free(destroy_user->group_identifier);
    }


    free(destroy_user);
}


struct Student* allocate_student(){
    struct Student *new_user = malloc(sizeof(struct Student));
    if(!new_user) return NULL;
    char *first_name_buffer = malloc(sizeof(char) * BUFSIZ);
    char *last_name_buffer = malloc(sizeof(char) * BUFSIZ);
    char *group_identifier_buffer = malloc(sizeof(char) * BUFSIZ);
    if(!last_name_buffer || !first_name_buffer || !group_identifier_buffer){
        destroy_student(new_user);
        return NULL;
    }
    new_user->first_name = first_name_buffer;
    new_user->last_name = last_name_buffer;
    new_user->group_identifier = group_identifier_buffer;
    return new_user;
}



