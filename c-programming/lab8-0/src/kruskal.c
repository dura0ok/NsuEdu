#include "kruskal.h"
#include <stdio.h>

#define LENGTH_SPAN_VERTEX -1

void swap(int* v1, int* v2) {
    int temp = *v1;
    *v1 = *v2;
    *v2 = temp;
}

int compare(const void* p1, const void* p2) {
    int arg1 = ((struct Node*)p1)->length;
    int arg2 = ((struct Node*)p2)->length;
    return  arg1 - arg2;
}

void init_sets(int* dsu, int numOfVertices) {
    for (int i = 1; i <= numOfVertices; i++) {
        dsu[i] = i;
    }
}

int find_set(int vertex, int* dsu) {
    if (dsu[vertex] == vertex) return vertex;
    dsu[vertex] = find_set(dsu[vertex], dsu);
    return dsu[vertex];
}

void union_set(int arg1, int arg2, int* dsu) {
    int arg1F = find_set(arg1, dsu);
    int arg2F = find_set(arg2, dsu);
    if (arg2F < arg1F) {
        swap(&arg1F, &arg2F);
    }
    dsu[arg2F] = arg1F;
}

void build_mst(int* dsu, struct Node* edges_array, int numOfEdges) {
    for (int i = 0; i < numOfEdges; i++) {
        if (find_set(edges_array[i].vertex1, dsu) != find_set(edges_array[i].vertex2, dsu)) {
            union_set(edges_array[i].vertex1, edges_array[i].vertex2, dsu);
            continue;
        }
        edges_array[i].length = LENGTH_SPAN_VERTEX;
    }
}


void print_solution(int* dsu, struct Node* edges_array, int edges_count, int vertex_count){
    for (int i = 2; i <= vertex_count; i++) {
        if (find_set(i - 1, dsu) != find_set(i, dsu)) {
            printf("no spanning tree");
            return;
        }
    }
    for (int i = 0; i < edges_count; i++) {
        if ((edges_array[i].length) != LENGTH_SPAN_VERTEX) {
            printf("%d %d\n", edges_array[i].vertex1, edges_array[i].vertex2);
        }
    }
}


