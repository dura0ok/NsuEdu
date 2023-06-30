#include "graph.h"
#include <stdlib.h>
#include <stdio.h>


void destroy_adjacency_matrix(int ** adjacency_matrix, int vertex_count){
    for (int i = 0; i < vertex_count; ++i) {
        free(adjacency_matrix[i]);
    }
    free(adjacency_matrix);
}

int** create_adjacency_matrix(int vertex_count){
    int **new_adjacency_matrix = malloc(sizeof(int*) * vertex_count);
    if(new_adjacency_matrix == NULL) return NULL;
    for (int i = 0; i < vertex_count; i++) {
        int *row = malloc(sizeof(int) * vertex_count);
        if(row == NULL){
            destroy_adjacency_matrix(new_adjacency_matrix, vertex_count);
            return NULL;
        }
        new_adjacency_matrix[i] = row;
        for (int j = 0; j < vertex_count; ++j) {
            new_adjacency_matrix[i][j] = 0;
        }
    }
    return new_adjacency_matrix;
}

struct Graph* create_graph(int start, int stop, int vertex_count, int edges_count){
    struct Graph *newGraph = malloc(sizeof(struct Graph));
    if(newGraph == NULL) return NULL;
    newGraph->start_index = start;
    newGraph->stop_index = stop;
    newGraph->edges_count = edges_count;
    newGraph->vertex_count = vertex_count;
    int** new_adjacency_matrix = create_adjacency_matrix(vertex_count);
    // TODO STRANGE ide warning
    if(new_adjacency_matrix == NULL){
        free(newGraph);

        return NULL;
    }
    newGraph->adjacency_matrix = new_adjacency_matrix;
    return newGraph;
}

void destroy_graph(struct Graph* graph){
    if(graph == NULL) return;
    destroy_adjacency_matrix(graph->adjacency_matrix, graph->vertex_count);
    free(graph);
}


