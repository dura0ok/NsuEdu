#include <stdio.h>
#include "stdbool.h"
#include "graph.h"
#include "limits.h"
#include "dijkstra.h"
#include "smart_buffer.h"
#include "string.h"
#include "stdlib.h"
#include "utils.h"

void bad_input(){
    printf("bad input");
}

bool in_range(int start, int stop, int data){
    if(data < start || data > stop) return false;
    return true;
}

bool validate_graph_info(int vertex_count, int edges_count, int start, int stop){
    if(!in_range(0, 5000, vertex_count)){
        printf("bad number of vertices");
        return false;
    }

    if(!in_range(0, vertex_count*(vertex_count-1)/2, edges_count)){
        printf("bad number of edges");
        return false;
    }

    if(!in_range(1, vertex_count, start) || !in_range(1, vertex_count, stop)){
        printf("bad vertex");
        return false;
    }
    return true;
}

void generate_table(const struct Graph *graph, struct SmartBuf *clever_buf) {
    int vertex_index = 1;
    push_char_array_to_buffer(clever_buf, "<tr>");
    push_char_array_to_buffer(clever_buf, "<th></th>");
    for (int i = 1; i <= graph->vertex_count; i++) {
        char temp_buf[BUFSIZ];
        snprintf(temp_buf, sizeof(temp_buf), "<th>%d</th>", i);
        push_char_array_to_buffer(clever_buf, temp_buf);
    }

    push_char_array_to_buffer(clever_buf, "</tr>");
    for (int i = 0; i < graph->vertex_count; ++i) {
        push_char_array_to_buffer(clever_buf, "<tr>");
        char temp_buf2[BUFSIZ];
        snprintf(temp_buf2, sizeof(temp_buf2), "<th>%d</th>", vertex_index);
        push_char_array_to_buffer(clever_buf, temp_buf2);
        for (int j = 0; j < graph->vertex_count; ++j) {
            char temp_buf[BUFSIZ];
            snprintf(temp_buf, sizeof(temp_buf), "<th>%d</th>", graph->adjacency_matrix[j][i]);
            push_char_array_to_buffer(clever_buf, temp_buf);
        }
        push_char_array_to_buffer(clever_buf, "</tr>");
        vertex_index++;
    }
}

int main(int argc, char * argv[]) {

    bool is_generate_html_mode = argc > 1 && strcmp(argv[1], "-p") == 0;

    int vertex_count, edges_count, start, stop;
    int start_edge, stop_edge, edge_length;
    if(scanf("%d", &vertex_count) != 1){
        bad_input();
        return 0;
    }

    if(scanf("%d %d", &start, &stop) != 2){
        bad_input();
        return 0;
    }

    if(scanf("%d", &edges_count) != 1){
        bad_input();
        return 0;
    }

    if(!validate_graph_info(vertex_count, edges_count, start, stop)){
        return 0;
    }

    struct Graph *graph = create_graph(start - 1, stop - 1, vertex_count, edges_count);
    int handle_line_counter = 0;
    for (int i = 0; i < edges_count; i++) {

        if(scanf("%d %d %d", &start_edge, &stop_edge, &edge_length) != 3){
            printf("bad line");
            destroy_graph(graph);
            return 0;
        }
        handle_line_counter++;
        if(!in_range(0, INT_MAX, edge_length)){
            printf("bad length");
            destroy_graph(graph);
            return 0;
        }

        graph->adjacency_matrix[start_edge-1][stop_edge-1] = edge_length;
        graph->adjacency_matrix[stop_edge-1][start_edge-1] = edge_length;
    }

    if(handle_line_counter != edges_count){
        printf("bad number of lines");
        destroy_graph(graph);
        return 0;
    }


    if(is_generate_html_mode){

        struct SmartBuf* clever_buf = create_smart_buffer();

        generate_table(graph, clever_buf);


        char *template_buf = read_file_content_to_buffer("template.txt");
        if(template_buf == NULL){
            return 0;
        }

        char *content = replace_substrings(template_buf, ":content", clever_buf->buf);

        printf("%s", content);
        free_smart_buffer(clever_buf);
        destroy_graph(graph);
        free(template_buf);
        free(content);
        return 0;
    }


    dijkstra(graph);

    destroy_graph(graph);
    return 0;
}


