#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <sys/poll.h>
#include "field.h"


struct Velocities {
    int dx;
    int dy;
};

//static char colors[5][10] = {"red", "green", "yellow", "blue", "white"};

struct Element {
    int x;
    int y;

    int x_prev;
    int y_prev;
};


struct Player {
    struct Element body[BUFSIZ];
    size_t length;
    size_t player_number;
    char last_move;
};


void update_food_cords(struct Game *game);
void init_player(struct Game *game, struct Player *player, size_t player_num);
void prepare_frame(struct Game *game);
struct Game init_board();
void update_field_player(struct Game *game, struct Player player);
void send_updated_game_to_clients(struct Game *game, const struct pollfd *fds, int nfds, struct Player players[2]);

void handleButton(struct Game *game, struct Player *player, char c);
struct Velocities getVelocities(char c);
int move(struct Game *game, struct Player *player, struct Velocities v);