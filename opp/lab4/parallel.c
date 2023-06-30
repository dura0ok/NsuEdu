#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <stdbool.h>

#include "mpi.h"
 //
#define SIZE_X 300
#define SIZE_Y 300
#define MAX_ITERATIONS 3500

void print_state(int * matrix, int sizeX, int sizeY) {
  for (int i = 0; i < sizeY; i++) {
    for (int j = 0; j < sizeX; j++) {
      printf("%d ", matrix[i * sizeX + j]);
    }
    printf("\n");
  }
  //    printf("\n");
}

void generate_glider(int * cur_state) { //Making first state called "Glider"
  for (int i = 0; i < SIZE_X * SIZE_Y; i++) {
    cur_state[i] = 0;
  }

  cur_state[0 * SIZE_X + 1] = true;
  cur_state[1 * SIZE_X + 2] = true;
  cur_state[2 * SIZE_X + 0] = true;
  cur_state[2 * SIZE_X + 1] = true;
  cur_state[2 * SIZE_X + 2] = true;
}

int count_neighbors(const int * board, int row, int col, int sizeX, int sizeY) {
  int count = 0;

  // Iterate through all the neighbors
  for (int j = row - 1; j <= row + 1; j++) {
    for (int i = col - 1; i <= col + 1; i++) {
      // Calculate the row and column indices of the current neighbor,
      // taking into account the toroidal nature of the grid
      int neighbor_col = (i + sizeX) % sizeX;
      int neighbor_row = (j + sizeY) % sizeY;

      // Check if the current neighbor is not the same as the given cell
      if (!(neighbor_row == row && neighbor_col == col)) {
        // If the neighbor is alive, increment the count
        if (board[neighbor_row * sizeX + neighbor_col] == 1) {
          count++;
        }
      }
    }
  }

  return count;
}

int count_top_neighbors(const int * board,
  const int * top, int row, int col, int sizeX, int sizeY) {
  int count = 0;
  int top_row = (row - 1 + sizeY) % sizeY;

  // Iterate through all the neighbors
  for (int j = row - 1; j <= row + 1; j++) {
    for (int i = col - 1; i <= col + 1; i++) {
      // Calculate the row and column indices of the current neighbor,
      // taking into account the toroidal nature of the grid
      int neighbor_col = (i + sizeX) % sizeX;
      int neighbor_row = (j + sizeY) % sizeY;

      // Check if the current neighbor is not the same as the given cell
      if (!(neighbor_row == row && neighbor_col == col)) {
        // If the neighbor is alive, increment the count
        if (neighbor_row == top_row) {
          if (top[neighbor_col] == 1) {
            count++;
          }
        } else if (board[neighbor_row * sizeX + neighbor_col] == 1) {
          count++;
        }
      }
    }
  }

  return count;
}

int count_bottom_neighbors(const int * board,
  const int * bottom, int row, int col, int sizeX, int sizeY) {
  int count = 0;
  int bottom_row = (row + 1) % sizeY;

  // Iterate through all the neighbors
  for (int j = row - 1; j <= row + 1; j++) {
    for (int i = col - 1; i <= col + 1; i++) {
      // Calculate the row and column indices of the current neighbor,
      // taking into account the toroidal nature of the grid
      int neighbor_col = (i + sizeX) % sizeX;
      int neighbor_row = (j + sizeY) % sizeY;

      // Check if the current neighbor is not the same as the given cell
      if (!(neighbor_row == row && neighbor_col == col)) {
        // If the neighbor is alive, increment the count
        if (neighbor_row == bottom_row) {
          if (bottom[neighbor_col] == 1) {
            count++;
          }
        } else if (board[neighbor_row * sizeX + neighbor_col] == 1) {
          count++;
        }
      }
    }
  }

  return count;
}

int main() {
  int rank, size;
  MPI_Init(NULL, NULL);
  double start_time = MPI_Wtime();
  MPI_Comm_size(MPI_COMM_WORLD, & size);
  MPI_Comm_rank(MPI_COMM_WORLD, & rank);
  int ** all_states = (int ** ) malloc(MAX_ITERATIONS * sizeof(int * ));
  int * flags = (int * ) malloc(MAX_ITERATIONS * sizeof(int));
  int * initial_state = (int * ) malloc(SIZE_X * SIZE_Y * sizeof(int));

  int rows_per_proc = SIZE_X / size;
  int remaining_rows = SIZE_X % size;
  int * send_counts = (int * ) malloc(size * sizeof(int));
  int * displs = (int * ) malloc(size * sizeof(int));

  int offset = 0;
  for (int i = 0; i < size; i++) {
    send_counts[i] = rows_per_proc * SIZE_X;
    if (remaining_rows > 0) {
      send_counts[i] += SIZE_X; // add an extra row to the first few processes
      remaining_rows--;
    }
    displs[i] = offset;
    offset += send_counts[i];
  }

  generate_glider(initial_state);

  int * curr_state = (int * ) malloc(send_counts[rank] * sizeof(int));

  MPI_Scatterv(initial_state, send_counts, displs, MPI_INT, curr_state, send_counts[rank], MPI_INT, 0,
    MPI_COMM_WORLD);
  int * top_line = (int * ) malloc(SIZE_X * sizeof(int));
  int * bottom_line = (int * ) malloc(SIZE_X * sizeof(int));
  int height = send_counts[rank] / SIZE_X;

  int size_x_small = SIZE_X;
  int size_y_small = height;
  //    if (rank == 2)print_state(curr_state, size_x_small, size_y_small);

  for (int iter = 0; iter < MAX_ITERATIONS; iter++) {
    all_states[iter] = curr_state;
    all_states[iter + 1] = (int * ) malloc(send_counts[rank] * sizeof(int));

    MPI_Request send_top, receive_bottom, send_bottom, receive_top;

    MPI_Isend( & all_states[iter][0], size_x_small, MPI_INT, (rank + size - 1) % size, 1, MPI_COMM_WORLD, & send_top);
    MPI_Isend( & all_states[iter][send_counts[rank] - size_x_small], size_x_small, MPI_INT, (rank + 1) % size, 2,
      MPI_COMM_WORLD, & send_bottom);
    MPI_Irecv(bottom_line, size_x_small, MPI_INT, (rank + 1) % size, 2, MPI_COMM_WORLD, & receive_bottom);
    MPI_Irecv(top_line, size_x_small, MPI_INT, (rank - 1 + size) % size, 1, MPI_COMM_WORLD, & receive_top);
    for (int i = 1; i < size_y_small - 1; i++) {
      for (int j = 0; j < size_x_small; j++) {
        int neighbors = count_neighbors(all_states[iter], i, j, size_x_small, size_y_small);
        //                if (rank == 0 && iter == 0 && j == 0) printf("%d", neighbors);

        if (all_states[iter][i * size_x_small + j] == 1 && (neighbors < 2 || neighbors > 3)) {
          all_states[iter + 1][i * size_x_small + j] = 0;
        } else if (all_states[iter][i * size_x_small + j] == 0 && neighbors == 3) {
          all_states[iter + 1][i * size_x_small + j] = 1;
        } else {
          all_states[iter + 1][i * size_x_small + j] = all_states[iter][i * size_x_small + j];
        }
      }
    }

    int flag1, flag2 = 0;
    MPI_Test( & request, & flag1, & receive_top);
    if (flag1) {
      for (size_t i = 0; i < size_x_small; i++) {
        int neighbors = count_top_neighbors(all_states[iter], top_line, 0, (int) i, size_x_small, size_y_small);
        if (rank == 0) printf("%d ", neighbors);

        if (all_states[iter][i] == 1 && (neighbors < 2 || neighbors > 3)) {
          all_states[iter + 1][i] = false;
        } else if (all_states[iter][i] == 0 && neighbors == 3) {
          all_states[iter + 1][i] = true;
        } else {
          all_states[iter + 1][i] = all_states[iter][i];
        }
      }
      MPI_Wait( & receive_bottom, MPI_STATUS_IGNORE);
    }

    MPI_Test( & request, & flag2, & receive_bottom);
    if (flag2) {

      for (size_t i = 0; i < size_x_small; i++) {
        int neighbors = count_bottom_neighbors(all_states[iter], bottom_line, size_y_small - 1, (int) i,
          size_x_small, size_y_small);

        if (all_states[iter][(size_y_small - 1) * SIZE_X + i] == 1 && (neighbors < 2 || neighbors > 3)) {
          all_states[iter + 1][(size_y_small - 1) * SIZE_X + i] = false;
        } else if (all_states[iter][(size_y_small - 1) * SIZE_X + i] == 0 && neighbors == 3) {
          all_states[iter + 1][(size_y_small - 1) * SIZE_X + i] = true;
        } else {
          all_states[iter + 1][(size_y_small - 1) * SIZE_X + i] = all_states[iter][(size_y_small - 1) * SIZE_X + i];
        }
      }

      MPI_Wait( & receive_top, MPI_STATUS_IGNORE);
    }

    curr_state = all_states[iter + 1];
    for (int back_step = iter; back_step >= 0; back_step--) {
      flags[back_step] = 1;
      for (int i = 0; i < size_y_small; i++) {
        for (int j = 0; j < SIZE_X; j++) {
          if (all_states[iter + 1][i * SIZE_X + j] != all_states[back_step][i * SIZE_X + j]) {
            flags[back_step] = 0;
            break;
          }
        }
        if (flags[back_step] == 0)
          break;
      }
    }

    MPI_Allreduce(MPI_IN_PLACE, flags, MAX_ITERATIONS, MPI_INT, MPI_LAND, MPI_COMM_WORLD);

    int check = 0;
    for (int i = 0; i < iter; i++) {
      if (flags[i] == 1) {
        check = 1;
        break;
      }
    }

    if (check == 1) {
      if (rank == 0) {
        printf("SOLVED %d \n", iter);
      }
      break;
    }

  }


  free(all_states);
  free(curr_state);

  double end_time = MPI_Wtime();
  if (rank == 0) {
    printf("%f\n", end_time - start_time);
  }

  MPI_Finalize();
}
