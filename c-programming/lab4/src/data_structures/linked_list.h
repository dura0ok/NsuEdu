#pragma once

#include "../token.h"

struct listNode {
    struct Token *data;
    struct listNode *next;
};

struct LinkedList {
    struct listNode *head;
};

struct LinkedList* createLinkedList();

void destroyList(struct LinkedList *list);

void displayList(struct LinkedList *list);

struct listNode *createListNode(struct Token *data);

void listPush(struct LinkedList *list, struct Token *data);


