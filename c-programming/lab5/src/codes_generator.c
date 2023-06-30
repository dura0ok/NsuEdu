#include <malloc.h>
#include <string.h>
#include "codes_generator.h"
#include "priority_queue.h"

#define LETTERS_COUNT 256

void generate_code(struct Code *codes, Node *node, struct Code code, unsigned char index) {

    if (node->left != NULL) {
        code.data[index] = '0';
        generate_code(codes, node->left, code, index + 1);
    }

    if (node->right != NULL) {
        code.data[index] = '1';
        generate_code(codes, node->right, code, index + 1);
    }

    if (node->left == NULL && node->right == NULL) {
        codes[node->element] = code;
        codes[node->element].size = index;
        // if one symbol in tree
        if (index == 0) codes[node->element].size = 1;
        return;
    }
}


struct Code *generate_codes_table(Node *root) {
    struct Code code = {.size = 0, .data = {'0'}};
    memset(code.data, '0', sizeof(code.data));
    struct Code *codes = malloc(sizeof(struct Code) * LETTERS_COUNT);
    for (int i = 0; i < LETTERS_COUNT; ++i) {
        codes[i].size = 0;
    }
    generate_code(codes, root, code, 0);
    //print_codes(codes);
    return codes;
}

