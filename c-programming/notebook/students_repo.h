#pragma once
#include "sqlite3.h"
#include "stdbool.h"

enum AnswerType {
    OK,
    ERROR,
    CANCEL
};

bool table_exists(sqlite3* db, const char* table_name);
bool create_student_table(sqlite3* db);