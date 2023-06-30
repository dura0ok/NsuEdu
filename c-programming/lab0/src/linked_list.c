#include "stdlib.h"
#include "stdio.h"
#include "linked_list.h"

struct Node * createNode(char data){
    struct Node * newNode = malloc(sizeof(struct Node));
    if(!newNode){
        return NULL;
    }
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

void push(struct LinkedList * list, char data){
    struct Node * current = NULL;
    if(list->head == NULL){
        list->head = createNode(data);
    }
    else {
        current = list->head;
        while (current->next!=NULL){
            current = current->next;
        }
        current->next = createNode(data);
    }
}

void display(struct LinkedList * list) {
    struct Node * current = list->head;
    if(current == NULL)
        return;

    for(; current != NULL; current = current->next) {
        printf("%c", current->data);
    }
}

void reverse(struct LinkedList * list){
    struct Node * reversed = NULL;
    struct Node * current = list->head;
    struct Node * temp = NULL;
    while(current != NULL){
        temp = current;
        current = current->next;
        temp->next = reversed;
        reversed = temp;
    }
    list->head = reversed;
}

void destroy(struct LinkedList * list){
    struct Node * current = list->head;
    struct Node * next;
    while(current != NULL){
        next = current->next;
        free(current);
        current = next;
    }
    //free(list->head);
}

