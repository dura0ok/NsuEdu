#include "client_engine.h"
#include "config.h"

#define X_OFFSET 100
#define Y_OFFSET 10

static const char* colors[MAX_CLIENTS] = {TC_YEL, TC_GRN, TC_RED, TC_BLU, TC_MAG};

void clearScreen() {
    printf("\033[0;0H\033[2J");
    fflush(stdout);
}

void drawCurrentState(struct Game game) {
    int cols;
    int rows;
    tc_get_cols_rows(&cols, &rows);
    tc_move_cursor(cols - X_OFFSET, rows / 2 - Y_OFFSET);


    printf("%s%s %s %s", TC_YEL, TC_BG_GRN, game.info_message, TC_NRM);

    tc_move_cursor(0, 0);

    for (size_t i = 0; i <= ROWS; ++i) {
        printf("--");
    }

    printf("\n");

    for (int i = 0; i < COLS; i++) {
        for (int j = 0; j < ROWS; j++) {
            if (j == 0) {
                printf("| ");
            }

            if(game.board[i][j] != ' ' && game.board[i][j] != FOOD_SYMBOL){
                int num = (int)game.board[i][j];
                int player_num = num;
                game.board[i][j] = BODY_SYMBOL;
                if(num > 100){
                    game.board[i][j] = HEAD_SYMBOL;
                    player_num -= 100;
                }
                printf("%s%c%s ", colors[player_num-1], game.board[i][j], TC_NRM);
            }else{
                printf("%c ", game.board[i][j]);
            }



            if (j == ROWS - 1) {
                printf("|");
            }
        }
        printf("\n");
    }

    for (size_t i = 0; i <= ROWS; ++i) {
        printf("--");
    }
    printf("\n");
}