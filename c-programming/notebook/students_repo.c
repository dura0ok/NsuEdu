#include "students_repo.h"
#include "string.h"
#include "stdio.h"
#include "student.h"

bool table_exists(sqlite3* db, const char* table_name){
    sqlite3_stmt* statement;
    int rc;

    const char* sql_table_list = "SELECT name FROM sqlite_master WHERE type='table'";

    rc = sqlite3_prepare_v2(db, sql_table_list, (int)strlen(sql_table_list), &statement, NULL);
    if(rc == SQLITE_OK) {
        // Loop through all the tables
        while(sqlite3_step(statement) == SQLITE_ROW) {
            if(!strcmp((const char*) sqlite3_column_text(statement, 0), table_name)){
                sqlite3_finalize(statement);
                return true;
            }
        }
    }

    sqlite3_finalize(statement);
    return false;
}

bool create_student_table(sqlite3* db){
    sqlite3_stmt* statement;
    int rc;

    // SQL query to create the 'user' table
    const char* sql_create_user_table = "CREATE TABLE users ("
                                        "id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,"
                                        "first_name TEXT NOT NULL,"
                                        "last_name TEXT NOT NULL,"
                                        "grade_level INTEGER NOT NULL,"
                                        "group_identifier TEXT NOT NULL,"
                                        "gpa REAL NOT NULL);";

    // Execute the previous query to create the 'user' table
    rc = sqlite3_prepare_v2(db, sql_create_user_table, (int)strlen(sql_create_user_table), &statement, NULL);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't initialize the database: %s\n", sqlite3_errmsg(db));
        return false;
    }

    sqlite3_step(statement);
    sqlite3_finalize(statement);


    return true;
}


bool insert_student(sqlite3* db, struct Student* user){
    sqlite3_stmt* statement;
    char sql_insert_user[BUFSIZ];
    sprintf(sql_insert_user,
            "INSERT INTO users (first_name, last_name, grade_level, group_identifier, gpa) VALUES ('%s', '%s', '%d', '%s', '%f')",
            user->first_name,
            user->last_name,
            user->grade_level,
            user->group_identifier,
            user->gpa
    );


    // Execute the previous query to insert a new line into the 'user' table
    int rc = sqlite3_prepare_v2(db, sql_insert_user, (int)strlen(sql_insert_user), &statement, NULL);

    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't initialize the database: %s\n", sqlite3_errmsg(db));
        return false;
    }

    sqlite3_step(statement);
    sqlite3_finalize(statement);

    return true;
}


