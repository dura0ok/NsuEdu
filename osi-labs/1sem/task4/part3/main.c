#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

#define PAGE_SIZE 4096
#define ALIGNMENT 8

typedef struct block {
    size_t size;
    struct block *next;
} block_t;

static block_t *head = NULL;

void *align_size(size_t size) {
    return (void *) ((size + ALIGNMENT - 1) / ALIGNMENT * ALIGNMENT);
}

void *mmap_new_block(size_t total_size) {
    total_size = PAGE_SIZE * ((total_size + PAGE_SIZE - 1) / PAGE_SIZE); // Round up to multiple of PAGE_SIZE
    void *block_start = mmap(NULL, total_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (block_start == MAP_FAILED) {
        return NULL;
    }
    block_t *block = (block_t *) block_start;
    block->size = total_size - sizeof(block_t);
    block->next = NULL;
    return block_start;
}

block_t *find_block(size_t total_size) {
    block_t *block = head;
    while (block != NULL) {
        if (block->size >= total_size) {
            return block;
        }
        block = block->next;
    }
    return NULL;
}

void remove_block(block_t *block) {
    if (block == head) {
        head = block->next;
    } else {
        block_t *prev_block = head;
        while (prev_block->next != block) {
            prev_block = prev_block->next;
        }
        prev_block->next = block->next;
    }
}

void *my_malloc(size_t size) {
    if (size == 0) {
        return NULL;
    }

    // Align size to multiple of ALIGNMENT
    size = (size_t) align_size(size);

    // Calculate total size required for block
    size_t total_size = size + sizeof(block_t);

    // Check if block already exists with enough space
    block_t *block = find_block(total_size);
    if (block != NULL) {
        // Block found with enough space
        void *block_start = (void *) block + sizeof(block_t);
        block->size -= total_size;
        if (block->size == 0) {
            // Block is completely used up, remove from list
            remove_block(block);
        }
        return block_start;
    }

    // No block found with enough space, allocate new block
    void *block_start = mmap_new_block(total_size);
    if (block_start == NULL) {
        return NULL;
    }
    block = (block_t *) block_start;
    block->size -= total_size;
    if (block->size == 0) {
        return block_start + sizeof(block_t);
    }

    if (head == NULL) {
        head = block;
    } else {
        block_t *last_block = head;
        while (last_block->next != NULL) {
            last_block = last_block->next;
        }
        last_block->next = block;
    }


    return block_start + sizeof(block_t);
}

void merge_blocks(block_t *block, block_t *prev_block) {
    // Mark block as free
    block->size += sizeof(block_t);
    if (prev_block == NULL) {
        // Block is at head of list
        head = block;
    } else {
        prev_block->next = block;
    }

    // Merge adjacent free blocks
    while (block->next != NULL && (void *) block + sizeof(block_t) + block->size == (void *) block->next) {
        block->size += sizeof(block_t) + block->next->size;
        block->next = block->next->next;
    }

    // Unmap block if it is completely free
    if (block->next == NULL && block->size >= PAGE_SIZE) {
        munmap(block, block->size);
    }
}

void my_free(void *ptr) {
    if (ptr == NULL) {
        return;
    }

    // Find block containing the pointer
    block_t *block = head;
    block_t *prev_block = NULL;
    while (block != NULL) {
        if ((void *) block + sizeof(block_t) == ptr) {
            break;
        }
        prev_block = block;
        block = block->next;
    }

    if (block == NULL) {
        fprintf(stderr, "error when free %p\n", ptr);
        return;
    }

    merge_blocks(block, prev_block);
}

int main() {
    void *ptr1 = my_malloc(20);
    void *ptr2 = my_malloc(20);
    printf("%p\n", ptr2);
    my_free(ptr2);
    void *ptr3 = my_malloc(20);
    void *ptr4 = my_malloc(20);
    printf("%p\n", ptr4);
    my_free(ptr4+2);
    return 0;
}