#pragma once

#include <stdio.h>
#include "priority_queue.h"

struct Code {
    size_t size;
    unsigned char data[256];
};

struct Code *generate_codes_table(Node *root);


