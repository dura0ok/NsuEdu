#include <stdio.h>
#include "dijkstra.h"
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include "graph.h"

int findMinEdgeFrom(int numOfVertices, long long *weights, bool* visited) {
    long long min = LLONG_MAX;
    int from = 0;
    for (int j = 0; j < numOfVertices; j++) {
        if (weights[j] < min && visited[j] != true) {
            min = weights[j];
            from = j;
        }
    }
    return from;
}

void print_solution(long long* weights, int* parents, int numOfVertex, bool requirementOverflow, int stop_index) {
    for (int i = 0; i < numOfVertex; i++) {
        if (weights[i] <= INT_MAX) {
            printf("%lli ", weights[i]);
            continue;
        }

        if (weights[i] != LLONG_MAX) {
            printf("INT_MAX+ ");
            continue;
        }
        printf("oo ");

    }
    printf("\n");
    if (requirementOverflow == true) {
        printf("overflow");
        return;
    }
    if (weights[stop_index] == LLONG_MAX) {
        printf("no path");
        return;
    }

    int current = stop_index;
    printf("%d ", stop_index + 1);
    while (current != parents[current]) {
        current = parents[current];
        printf("%d ", current + 1);
    }
}

void dijkstra(struct Graph *graph){
    bool *visited = malloc(graph->vertex_count * sizeof(bool));
    long long *weights = malloc(graph->vertex_count * sizeof(long long));
    int *parents = malloc(graph->vertex_count * sizeof(int));

    for (int i = 0; i < graph->vertex_count; i++){
        weights[i] = LLONG_MAX;
        visited[i] = false;
        parents[i] = i;
    }

    weights[graph->start_index] = 0;
    bool requirementOverflow = false;
    int current = graph->start_index;
    for (int i = 0; i < graph->vertex_count; i++) {
        visited[current] = true;
        for (int j = 0; j < graph->vertex_count; j++) {
            long long new_path = weights[current] + (long long)graph->adjacency_matrix[current][j];
            if (graph->adjacency_matrix[current][j] && weights[j] >= new_path && weights[j] && !visited[j]) {
                if (weights[j] > INT_MAX && weights[j] != LLONG_MAX && j == graph->stop_index) {
                    requirementOverflow = true;
                }
                weights[j] = weights[current] + (long long )graph->adjacency_matrix[current][j];

                parents[j] = current;
            }
        }
        current = findMinEdgeFrom(graph->vertex_count, weights, visited);
    }

    print_solution(weights, parents, graph->vertex_count, requirementOverflow, graph->stop_index);
    free(parents);
    free(visited);
    free(weights);
}


