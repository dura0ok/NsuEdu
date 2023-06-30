#include <stdio.h>
#include "priority_queue.h"
#include <malloc.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "codes_generator.h"
#include "utils.h"
#include "bit_file.h"


#define LETTERS_COUNT 256
#define DEFAULT_FILE_NAME "huff.huff"


size_t *generate_frequency_table(FILE *encoding_file) {
    unsigned char buf[BUFSIZ];
    size_t *frequency_table = calloc(LETTERS_COUNT, sizeof(size_t));
    while (1) {
        size_t n = fread(buf, 1, BUFSIZ, encoding_file);
        if (n == 0) break;
        for (size_t i = 0; i < n; i++) {
            frequency_table[(unsigned char) buf[i]]++;
        }
    }

    return frequency_table;
}

void build_tree(PriorityQueue *q) {

    while (size(q) > 1) {
        Node *first_dequeued_element, *second_dequeued_element, *union_element;
        first_dequeued_element = dequeue(q);
        second_dequeued_element = dequeue(q);

        union_element = create_node(EOF, first_dequeued_element->priority + second_dequeued_element->priority,
                                    first_dequeued_element, second_dequeued_element);
        enqueue(q, union_element);
    }
}


void push_letters_to_queue(const size_t *table, PriorityQueue *q) {
    for (size_t i = 0; i < LETTERS_COUNT; ++i) {
        if (table[i] == 0) continue;
        Node *item = create_node((int) i, table[i], NULL, NULL);
        enqueue(q, item);
    }
}

void write_huffman_tree_to_file(FILE *new_encoding_file, struct Node *node, struct T_smart_byte *byte) {
    if (node->left != NULL || node->right != NULL) {
        set_bit(byte, 0, new_encoding_file);

        if (node->left != NULL) {
            write_huffman_tree_to_file(new_encoding_file, node->left, byte);
        }
        if (node->right != NULL) {
            write_huffman_tree_to_file(new_encoding_file, node->right, byte);
        }
    }


    if (node->left == NULL && node->right == NULL) {
        set_bit(byte, 1, new_encoding_file);
        char symbol = (char) node->element;
        for (int i = 0; i < 8; ++i) {
            set_bit(byte, ((symbol & 128) == 0) ? 0 : 1, new_encoding_file);
            symbol <<= 1;
        }

    }

}

void encode_buffer(FILE *new_encoding_file, const unsigned char buf[BUFSIZ], struct T_smart_byte *bit, size_t n,
                   struct Code *codes) {
    for (size_t i = 0; i < n; ++i) {
        unsigned char c = (unsigned char) buf[i];
        size_t size = codes[c].size;
        for (size_t j = 0; j < size; ++j) {
            set_bit(bit, codes[c].data[j] - '0', new_encoding_file);
        }
    }
}

void write_data_to_file(FILE *file_to_encode, FILE *new_encoding_file, struct T_smart_byte *bit, struct Code *codes) {
    unsigned char buf[BUFSIZ];
    while (1) {
        size_t n = fread(buf, 1, BUFSIZ, file_to_encode);
        if (n == 0) break;
        encode_buffer(new_encoding_file, buf, bit, n, codes);
    }
}

void encoder(FILE *file_to_encode, struct Node *root, bool testing_mode, FILE *encoded_file) {

    if (root == NULL) {
        return;
    }
    struct Code *codes = generate_codes_table(root);
    struct T_smart_byte byte = {.busy_size = 0};
    long position = ftell(encoded_file);
    putc(' ', encoded_file);


    if (root->left == NULL && root->right == NULL) write_huffman_tree_to_file(encoded_file, root, &byte);
    if (root->left != NULL) write_huffman_tree_to_file(encoded_file, root->left, &byte);
    if (root->right != NULL) write_huffman_tree_to_file(encoded_file, root->right, &byte);
    pull_bit(&byte, encoded_file);

    long offset = testing_mode ? 1 : 0;
    fseek(file_to_encode, offset, SEEK_SET);

    write_data_to_file(file_to_encode, encoded_file, &byte, codes);
    char num_of_significant_bits = (char) (8 - byte.busy_size);
    pull_bit(&byte, encoded_file);
    fseek(encoded_file, position, 0);
    putc(num_of_significant_bits, encoded_file);

    free(codes);
}

struct Node *restore_tree(T_bit_file *bf, struct T_smart_byte *byte, size_t *garbage) {
    int bit = f_get_bit(bf);
    if (bit) {
        for (int i = 0; i < 8; ++i) {
            int tmp_bit = f_get_bit(bf);
            set_bit(byte, tmp_bit, NULL);
        }

        (*garbage)++;
        return create_node(byte->data, EOF, NULL, NULL);
    }
    struct Node *node = create_node(EOF, -1, NULL, NULL);

    (*garbage)++;
    node->left = restore_tree(bf, byte, garbage);
    node->right = restore_tree(bf, byte, garbage);

    return node;
}


struct Node *decode_tree(T_bit_file *bf) {
    struct Node *root = create_node(EOF, 0, NULL, NULL);
    struct T_smart_byte byte = {.busy_size = 0};

    size_t garbage = 0;
    root->left = restore_tree(bf, &byte, &garbage);
    root->right = restore_tree(bf, &byte, &garbage);

    if (garbage % 8 != 0) {
        for (size_t i = 0; i < 8 - (garbage % 8); ++i) {
            f_get_bit(bf);
        }
    }

    byte.busy_size = 0;
    return root;
}

// 0 rb, 1 wb
FILE *open_file(const char *path_file_to_decode, bool mode) {
    FILE *fp;
    if (!mode) {
        fp = fopen(path_file_to_decode, "rb");
    } else {
        fp = fopen(path_file_to_decode, "wb");
    }
    assert(fp != NULL);
    return fp;
}

void walk(struct Node **walker, int bit, struct Node *root, FILE *out) {
    if (bit) {
        *walker = (*walker)->right;
    } else {
        *walker = (*walker)->left;
    }
    if ((*walker)->left == NULL && (*walker)->right == NULL) {
        if (out != NULL) {
            putc((*walker)->element, out);
        } else {
            putc((*walker)->element, stdout);
        }

        *walker = root;
    }

}

void encode(FILE *file_to_encode, bool testing_mode, FILE *encoded_file) {
    size_t *table = generate_frequency_table(file_to_encode);

    PriorityQueue *q = newPriorityQueue(LETTERS_COUNT);
    push_letters_to_queue(table, q);
    if (size(q) == 1) {
        struct Node *copy_node = create_node(front(q)->element, front(q)->priority, front(q)->left, front(q)->right);
        enqueue(q, copy_node);
    }
    build_tree(q);

    struct Node *root = front(q);
    encoder(file_to_encode, root, testing_mode, encoded_file);

    free(table);
    free_queue(q);

}

long get_end_byte_position(FILE *fp) {
    long current = ftell(fp);
    fseek(fp, 0, SEEK_END);
    long end_byte_position = ftell(fp);
    fseek(fp, current, SEEK_SET);
    return end_byte_position;
}

void free_tree(struct Node *root) {
    if (root != NULL) {
        free_tree(root->left);
        free_tree(root->right);
        free(root);
    }
}

void decode(FILE *file_from_decode, FILE *to_decode) {
    int num_of_significant_bits = fgetc(file_from_decode);
    if (num_of_significant_bits == EOF) {
        return;
    }
    T_bit_file *bf = bf_create(file_from_decode);


    long end_byte_position = get_end_byte_position(file_from_decode);
    struct Node *root = decode_tree(bf);
    struct Node *walker = root;

    char cnt = 0;
    while (1) {
        int bit = f_get_bit(bf);
        if (EOF == bit) break;
        walk(&walker, bit, root, to_decode);
        if (ftell(bf->fp) == end_byte_position) {
            cnt++;
            if (cnt == (8 - num_of_significant_bits)) break;
        }
    }


    free_tree(root);
    bf_close(bf);

}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        FILE *from_file = open_file("in.txt", 0);
        int mode = fgetc(from_file);
        if ((char) mode == 'c') {
            FILE *to_file = open_file("out.txt", 1);
            encode(from_file, true, to_file);
            fclose(from_file);
        } else if ((char) mode == 'd') {
            FILE *to_file = open_file("out.txt", 1);
            decode(from_file, to_file);
            fclose(to_file);
        } else {
            assert(0);
        }

        return 0;
    }


    bool is_encoder_mode = argc > 3 && strcmp(argv[1], "-c") == 0;
    bool is_decoder_mode = argc > 3 && strcmp(argv[1], "-d") == 0;
    if (!(is_encoder_mode || is_decoder_mode)) return 0;

    char *from_path = argv[2];
    FILE *from_file = open_file(from_path, 0);

    char *to_path = argv[3];
    FILE *to_file = open_file(to_path, 1);


    if (is_encoder_mode) {
        encode(from_file, false, to_file);
        fclose(from_file);
    }

    if (is_decoder_mode) {
        decode(from_file, to_file);
    }


    fclose(to_file);


    return 0;
}


