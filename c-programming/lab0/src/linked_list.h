#ifndef LABA_LINKED_LIST_H
#define LABA_LINKED_LIST_H


struct Node {
    char data;
    struct Node * next;
};

struct LinkedList {
    struct Node * head;
};

void reverse(struct LinkedList * list);
void destroy(struct LinkedList * list);
void display(struct LinkedList * list);
struct Node * createNode(char data);
void push(struct LinkedList * list, char data);


#endif

