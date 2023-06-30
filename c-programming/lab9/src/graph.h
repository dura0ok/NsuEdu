#pragma once

struct Graph{
    int start_index;
    int stop_index;
    int edges_count;
    int vertex_count;
    int **adjacency_matrix;
};

void destroy_adjacency_matrix(int ** adjacency_matrix, int vertex_count);
int** create_adjacency_matrix(int vertex_count);
struct Graph* create_graph(int start, int stop, int vertex_count, int edges_count);
void destroy_graph(struct Graph* graph);

void dijkstra(struct Graph* graph);




