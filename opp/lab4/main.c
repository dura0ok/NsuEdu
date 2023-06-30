#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define SIZE_X 100
#define SIZE_Y 100
#define MAX_ITERATIONS 1000

void print_state(int *matrix) {
    for (int i = 0; i < SIZE_X; i++) {
        for (int j = 0; j < SIZE_Y; j++) {
            printf("%d ", matrix[i * SIZE_X + j]);
        }
        printf("\n");
    }
    printf("\n");
}

void generate_glider(int *cur_state) {
    for (int i = 0; i < SIZE_X * SIZE_Y; i++) {
        cur_state[i] = 0;
    }

    cur_state[0 * SIZE_X + 1] = true;
    cur_state[1 * SIZE_X + 2] = true;
    cur_state[2 * SIZE_X + 0] = true;
    cur_state[2 * SIZE_X + 1] = true;
    cur_state[2 * SIZE_X + 2] = true;
}

int count_neighbors(const int *board, int row, int col) {
    int count = 0;

    // Iterate through all the neighbors
    for (int i = row-1; i <= row+1; i++) {
        for (int j = col-1; j <= col+1; j++) {
            // Calculate the row and column indices of the current neighbor,
            // taking into account the toroidal nature of the grid
            int neighbor_row = (i + SIZE_X) % SIZE_X;
            int neighbor_col = (j + SIZE_Y) % SIZE_Y;

            // Check if the current neighbor is not the same as the given cell
            if (!(neighbor_row == row && neighbor_col == col)) {
                // If the neighbor is alive, increment the count
                if (board[neighbor_row * SIZE_X + neighbor_col] == 1) {
                    count++;
                }
            }
        }
    }

    return count;
}


void update_board(int *new_board, int *board){
    int neighbors;

    // Iterating through all cells
    for (int i = 0; i < SIZE_X; i++) {
        for (int j = 0; j < SIZE_Y; j++) {
            // Counting the living neighbors
            neighbors = count_neighbors(board, i, j);

            // Applying the game rules
            if (board[i* SIZE_X + j] == 1 && (neighbors < 2 || neighbors > 3)) {
                new_board[i * SIZE_X + j] = 0;
            }
            else if (board[i * SIZE_X + j] == 0 && neighbors == 3) {
                new_board[i * SIZE_X + j] = 1;
            }
            else {
                new_board[i * SIZE_X + j] = board[i * SIZE_X + j];
            }
        }
    }
}

int main() {
    int **all_states = (int **) malloc((MAX_ITERATIONS + 1) * sizeof(int *));
    int *cur_state = (int *) malloc(SIZE_X * SIZE_Y * sizeof(int));
    generate_glider(cur_state);

    //print_state(cur_state);
    //printf("%d\n", count_neighbors(cur_state, 0, 0));

    for (int iter = 0; iter < MAX_ITERATIONS; iter++){
        //print_state(cur_state);
        all_states[iter] = cur_state;
        all_states[iter+1] = (int *) malloc(SIZE_X * SIZE_Y * sizeof(int));
        update_board(all_states[iter+1], cur_state);

        int check = 1;

        for (int back_step = iter; back_step >= 0; back_step--) {
            if (memcmp(all_states[iter+1], all_states[back_step], SIZE_X * SIZE_Y * sizeof(int)) == 0) {
                printf("%d\n", back_step);
                check = 0;
                break;
            }
        }

        if (check == 0) {
            printf("ITER %d\n", iter);
            break;
        }

        cur_state = all_states[iter+1];
    }

    free(all_states);
    free(cur_state);
}


