#pragma once

struct Node {
    short int vertex1;
    short int vertex2;
    int length;
};

int compare(const void* p1, const void* p2);
void init_sets(int* dsu, int numOfVertices);
void build_mst(int* dsu, struct Node* arrayToSort, int numOfEdges);
void print_solution(int* dsu, struct Node* edges_array, int edges_count, int vertex_count);


