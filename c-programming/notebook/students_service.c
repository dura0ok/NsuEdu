#include "students_service.h"
#include "stdio.h"
#include "string.h"
#include "utils.h"
#include "smart_buffer.h"
#include "student.h"
#include "students_repo.h"

enum AnswerType create_student(sqlite3 *db) {
    struct Student *new_user = allocate_student();

    char inputBuf[BUFSIZ];

    printf("Enter First name: ");
    fgets(inputBuf, BUFSIZ, stdin);
    remove_new_line(inputBuf);
    strcpy(new_user->first_name, inputBuf);

    printf("Enter last name: ");
    fgets(inputBuf, BUFSIZ, stdin);
    remove_new_line(inputBuf);
    strcpy(new_user->last_name, inputBuf);

    printf("Enter grade level: ");
    int grade_level;
    if (scanf("%d", &grade_level) != 1) {
        destroy_student(new_user);
        return ERROR;
    }

    getchar();


    new_user->grade_level = grade_level;

    inputBuf[strlen(inputBuf) - 1] = 0;
    memset(inputBuf, 0, sizeof(inputBuf));
    printf("Enter group name: ");
    fgets(inputBuf, BUFSIZ, stdin);
    remove_new_line(inputBuf);
    strcpy(new_user->group_identifier, inputBuf);

    inputBuf[strlen(inputBuf) - 1] = 0;


    double gpa;
    printf("Enter GPA: ");
    if (scanf("%lf", &gpa) != 1) {
        destroy_student(new_user);
        return ERROR;
    }
    new_user->gpa = gpa;

    getchar();


    if (!insert_student(db, new_user)) {
        sqlite3_close(db);
        return ERROR;
    }

    destroy_student(new_user);


    return OK;
}


void print_rows(sqlite3_stmt *statement) {
    size_t rows_counter = 0;
    while (sqlite3_step(statement) == SQLITE_ROW) {
        struct Student user = {0};
        user.first_name = (char *) sqlite3_column_text(statement, 1);
        user.last_name = (char *) sqlite3_column_text(statement, 2);
        user.grade_level = (int) sqlite3_column_int(statement, 3);
        user.group_identifier = (char *) sqlite3_column_text(statement, 4);
        user.gpa = (double) sqlite3_column_double(statement, 5);
        printf("%d %s %s %d %s %f\n", sqlite3_column_int(statement, 0), user.first_name, user.last_name, user.grade_level, user.group_identifier, user.gpa);
        printf("--------------------------------\n");
        rows_counter++;
    }
    if(rows_counter == 0){
        printf("no rows\n");
    }
}

void display_users(sqlite3 *db) {
    sqlite3_stmt *statement;
    const char *sql_display_users = "SELECT * FROM users;";
    int rc = sqlite3_prepare_v2(db, sql_display_users, (int) strlen(sql_display_users), &statement, NULL);

    if (rc == SQLITE_OK) {
        print_rows(statement);
    }

    sqlite3_finalize(statement);
}

void find_by_column(sqlite3 *db) {
    char column_name[BUFSIZ];
    char column_value[BUFSIZ];
    printf("enter column name: ");
    fgets(column_name, BUFSIZ, stdin);
    printf("enter column value: ");
    fgets(column_value, BUFSIZ, stdin);
    remove_new_line(column_name);
    remove_new_line(column_value);

    struct SmartBuf *clever_buf = create_smart_buffer();
    push_char_array_to_buffer(clever_buf, "SELECT * FROM users WHERE :column=':value'");
    clever_buf->buf = replace_substrings(clever_buf->buf, ":column", column_name);
    clever_buf->buf = replace_substrings(clever_buf->buf, ":value", column_value);


    sqlite3_stmt *statement;
    int rc = sqlite3_prepare_v2(db, clever_buf->buf, (int) strlen(clever_buf->buf), &statement, NULL);
    free_smart_buffer(clever_buf);

    if (rc == SQLITE_OK) {
        print_rows(statement);
    }

    sqlite3_finalize(statement);
}


enum AnswerType delete_user(sqlite3 *db){
    char *zErrMsg = 0;
    char column_name[BUFSIZ];
    char column_value[BUFSIZ];
    printf("enter column name: ");
    fgets(column_name, BUFSIZ, stdin);
    printf("enter column value: ");
    fgets(column_value, BUFSIZ, stdin);
    remove_new_line(column_name);
    remove_new_line(column_value);


    struct SmartBuf *clever_buf = create_smart_buffer();
    push_char_array_to_buffer(clever_buf, "DELETE FROM users WHERE :column=:value");
    clever_buf->buf = replace_substrings(clever_buf->buf, ":column", column_name);
    clever_buf->buf = replace_substrings(clever_buf->buf, ":value", column_value);

    int rc = sqlite3_exec(db, clever_buf->buf, NULL, NULL, &zErrMsg);

    if(rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        return ERROR;
    }

    return OK;
}