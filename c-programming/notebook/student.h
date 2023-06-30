#include <sqlite3.h>
#include "stdbool.h"

struct Student{
    char *first_name;
    char *last_name;
    int grade_level;
    char *group_identifier;
    double gpa;
};

void destroy_student(struct Student *destroy_user);
struct Student* allocate_student();
bool insert_student(sqlite3* db, struct Student* user);