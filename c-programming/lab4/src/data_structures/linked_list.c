#include "stdlib.h"
#include "stdio.h"
#include "linked_list.h"

struct LinkedList* createLinkedList(){
    struct LinkedList* newLinkedList = malloc(sizeof(struct LinkedList));
    if(newLinkedList == NULL) return NULL;
    newLinkedList->head = NULL;
    return newLinkedList;
}

struct listNode *createListNode(struct Token *data) {
    struct listNode *newNode = malloc(sizeof(struct listNode));
    if (!newNode) {
        return NULL;
    }
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

void listPush(struct LinkedList *list, struct Token *data) {
    struct listNode *current = NULL;
    if (list->head == NULL) {
        list->head = createListNode(data);
    } else {
        current = list->head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = createListNode(data);
    }
}

void displayList(struct LinkedList *list) {
    struct listNode *current = list->head;
    if (current == NULL)
        return;

    for (; current != NULL; current = current->next) {
        if (current->data->type == OPERATOR) {
            printf("%c ", current->data->operator);
        } else {
            printf("%d ", current->data->operand);
        }
    }
}

void destroyList(struct LinkedList *list) {
    struct listNode* tmp;

    while (list->head != NULL){
        tmp = list->head;
        list->head = list->head->next;

        if(tmp->data->type == OPERAND){
            free(tmp->data);
        }
        free(tmp);
    }
    free(list);
}


