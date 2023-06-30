#include <stdio.h>
#include "stdbool.h"
#include "string.h"
#include "sqlite3.h"
#include "students_repo.h"
#include "students_service.h"
#include "utils.h"


enum AnswerType handle_input(sqlite3 *db) {
    char buffer[BUFSIZ];
    if (feof(stdin)) {
        return 0;
    }
    printf("enter your command: ");

    if (!fgets(buffer, BUFSIZ, stdin))
        return ERROR;

    remove_new_line(buffer);
    if (strcmp(buffer, "exit") == 0) {
        return CANCEL;
    }


    if (strcmp(buffer, "display") == 0) {
        display_users(db);
    }

    if (strcmp(buffer, "add") == 0) {
        if(create_student(db) == ERROR){
            printf("error\n");
        }else{
            printf("user successfully inserted\n");
        }

    }

    if (strcmp(buffer, "delete") == 0) {
        if(delete_user(db) == ERROR){
            printf("error\n");
        }else{
            printf("user successfully deleted\n");
        }
    }


    if (strcmp(buffer, "find") == 0) {
        find_by_column(db);
    }

    if (strcmp(buffer, "help") == 0) {
        printf("add, display, delete find commands\n");
    }

    return OK;
}



int main(int argc, char *argv[]) {
    const char *DATABASE_NAME = "students.db";


    bool isInteractive = false;
    if (argc > 1 && strcmp(argv[1], "-i") == 0) {
        isInteractive = true;
    }


    sqlite3 *db;
    int rc = sqlite3_open(DATABASE_NAME, &db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    if (!table_exists(db, "users")) {
        if (!create_student_table(db)) {
            sqlite3_close(db);
            return 1;
        }
    }

    if (isInteractive) {
        while (1) {

            int result = handle_input(db);
            if (result == CANCEL) break;
        }

    } else {
        handle_input(db);
    }

    sqlite3_close(db);

    return 0;
}
