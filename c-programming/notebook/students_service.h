#pragma once
#include "sqlite3.h"

enum AnswerType create_student(sqlite3 *db);
void display_users(sqlite3 *db);
void find_by_column(sqlite3 *db);
enum AnswerType delete_user(sqlite3 *db);