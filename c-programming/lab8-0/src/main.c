#include <stdio.h>
#include <stdbool.h>
#include "kruskal.h"
#include <stdlib.h>
#include <limits.h>

void bad_input(){
    printf("bad input");
}

bool in_range(int start, int stop, int data){
    if(data < start || data > stop) return false;
    return true;
}

bool validate_graph_info(int vertex_count, int edges_count){

    if (edges_count == 0 && vertex_count != 1) {
        printf("no spanning tree");
        return 0;
    }

    if(!in_range(0, 5000, vertex_count)){
        printf("bad number of vertices");
        return false;
    }

    if(!in_range(0, vertex_count*(vertex_count-1)/2, edges_count)){
        printf("bad number of edges");
        return false;
    }

    return true;
}


int main() {
    int vertex_count, edges_count;
    int start_edge, stop_edge, edge_length;

    if(scanf("%d", &vertex_count) != 1){
        bad_input();
        return 0;
    }

    if(scanf("%d", &edges_count) != 1){
        bad_input();
        return 0;
    }

    if(!validate_graph_info(vertex_count, edges_count)){
        return 0;
    }

    int handle_lines_counter = 0;

    struct Node *edges_array = malloc(edges_count * sizeof(struct Node));

    for (int i = 0; i < edges_count; ++i) {
        if(scanf("%d %d %d", &start_edge, &stop_edge, &edge_length) != 3){
            if(handle_lines_counter != edges_count){
                printf("bad number of lines");
            }else{
                printf("bad line");
            }
            free(edges_array);
            return 0;
        }

        if(!in_range(0, INT_MAX, edge_length)){
            printf("bad length");
            free(edges_array);
            return 0;
        }

        if(!in_range(1, vertex_count, start_edge) || !in_range(1, vertex_count, stop_edge)){
            printf("bad vertex");
            free(edges_array);
            return 0;
        }
        handle_lines_counter++;
        edges_array[i].vertex1 = (short int)start_edge;
        edges_array[i].vertex2 = (short int)stop_edge;
        edges_array[i].length = edge_length;
    }



    qsort(edges_array, edges_count, sizeof(edges_array[0]), compare);

    int* dsu = (int*)malloc((vertex_count + 1) * sizeof(int));
    init_sets(dsu, vertex_count);
    build_mst(dsu, edges_array, edges_count);

    print_solution(dsu, edges_array, edges_count, vertex_count);
    free(edges_array);
    free(dsu);


    return 0;
}


