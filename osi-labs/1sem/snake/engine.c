#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "engine.h"
#include "config.h"

int generate_random_in_range(int min, int max) {
    int range = max - min + 1;
    return rand() % range + min;
}



void update_food_cords(struct Game *game) {
    int x, y;
    printf("occupied cells %zu\n", game->count_occupied_cells);
    size_t free_cells = COLS*ROWS - game->count_occupied_cells;


    int random_skip = generate_random_in_range(0, (int)free_cells - 1);
    int current_counter = 0;
    for(size_t i = 0; i < ROWS; i++){
        for(size_t j = 0; j < COLS; j++){
            if(game->board[i][j] == ' '){
                current_counter++;
            }

            if(random_skip + 1 == current_counter){
                x = (int)j;
                y = (int)i;
                break;
            }
        }
        if(random_skip + 1 == current_counter){
            break;
        }
    }

    game->food.x = x;
    game->food.y = y;

}

void init_player(struct Game *game, struct Player *player, size_t player_num) {
    int x,y;
    do {
        x = generate_random_in_range(0, COLS - 1);
        y = generate_random_in_range(0, ROWS - 1);
    } while (game->board[x][y] != ' ');

    player->body[0].x = x;
    player->body[0].y = y;

    player->body[0].x_prev = 0;
    player->body[0].y_prev = 0;
    player->length = 1;
    player->player_number = player_num;
    player->last_move = 'x';
}

void prepare_frame(struct Game *game) {
    memset(game->board, ' ', COLS * ROWS * sizeof(char));
    printf("prepare frame food coords (%d %d)\n", game->food.x, game->food.y);
    game->board[game->food.x][game->food.y] = FOOD_SYMBOL;
}

struct Game init_board() {
    struct Game game;
    memset(game.board, ' ', COLS * ROWS * sizeof(char));


    game.count_occupied_cells = 1;
    update_food_cords(&game);
    game.is_running = true;
    return game;
}

void update_field_player(struct Game *game, struct Player player) {
   
    char symbol = (char)(100 + player.player_number);
    game->board[player.body[0].x][player.body[0].y] = symbol;
    symbol = player.player_number;
    for (size_t i = 1; i < player.length; i++) {
        game->board[player.body[i].x][player.body[i].y] = symbol;
    }
}

void handleButton(struct Game *game, struct Player *player, char c) {
    int res = move(game, player, getVelocities(c));
    if(res == 0){
        player->last_move = c;
    }
}

void send_updated_game_to_clients(struct Game *game, const struct pollfd *fds, int nfds, struct Player players[MAX_CLIENTS]){
    prepare_frame(game);

    for(size_t  i = 0; i < nfds - 1; i++){
        update_field_player(game, players[i]);
    }



    for (int j = 1; j <= nfds -1; j++) {
        if(COLS*ROWS - game->count_occupied_cells == 0){
            game->is_running = false;
            sprintf(game->info_message, "Draw");
            continue;
        }

        if(game->is_running){
            sprintf(game->info_message, "Game is running. Player %zu", players[j-1].player_number);
        }

        if (send(fds[j].fd, game, sizeof(struct Game), 0) == -1) {
            perror("send");
            continue;
        }
    }
}

struct Velocities getVelocities(char c) {
    struct Velocities res = {};
    if (c == 'w') {
        res.dx = -1;
        res.dy = 0;
    } else if (c == 'a') {
        res.dx = 0;
        res.dy = -1;
    } else if (c == 's') {
        res.dx = 1;
        res.dy = 0;
    } else if (c == 'd') {
        res.dx = 0;
        res.dy = 1;
    } else {
        res.dx = 0;
        res.dy = 0;
    }

    return res;
}

int move(struct Game *game, struct Player *player, struct Velocities v) {
    int res = 0;
    if (player->length > 1) {
        if (player->body[0].x + v.dx == player->body[1].x && player->body[0].y + v.dy == player->body[1].y) {
            v = getVelocities(player->last_move);
            res = 1;
        }
    }

    player->body[0].x_prev = player->body[0].x;
    player->body[0].y_prev = player->body[0].y;

    player->body[0].x += v.dx;
    player->body[0].y += v.dy;

    if (player->body[0].x >= COLS) {
        player->body[0].x = 0;
    }

    if (player->body[0].x < 0) {
        player->body[0].x = COLS - 1;
    }

    if (player->body[0].y < 0) {
        player->body[0].y = ROWS - 1;
    }

    if (player->body[0].y >= ROWS) {
        player->body[0].y = 0;
    }


    for (size_t i = 1; i < player->length; i++) {
        player->body[i].x_prev = player->body[i].x;
        player->body[i].y_prev = player->body[i].y;

        player->body[i].x = player->body[i - 1].x_prev;
        player->body[i].y = player->body[i - 1].y_prev;

    }

    for (size_t i = 1; i < player->length; i++) {
        if (player->body[0].x == player->body[i].x && player->body[0].y == player->body[i].y) {
            game->is_running = false;

            sprintf(game->info_message, "Player %zu lose. Other wins", player->player_number);
            break;
        }
    }

    if (player->body[0].x == game->food.x && player->body[0].y == game->food.y) {
        player->body[player->length].x = player->body[player->length - 1].x_prev;
        player->body[player->length].y = player->body[player->length - 1].y_prev;
        player->length++;
        game->count_occupied_cells++;


        update_food_cords(game);

    }

    return res;
}
