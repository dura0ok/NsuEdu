#pragma once
#include <stdio.h>
#include <stdbool.h>
#define ROWS 30
#define COLS 30
#define EXIT_MESSAGE_BUFFER_SIZE 128

struct Food {
    int x;
    int y;
    char symbol;
};

struct Game {
    char board[COLS][ROWS];
    struct Food food;
    bool is_running;
    char info_message[EXIT_MESSAGE_BUFFER_SIZE];
    size_t count_occupied_cells;
};