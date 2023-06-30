#pragma once

#include <stdbool.h>
#include "../token.h"
#include "linked_list.h"

struct Stack {
    struct StackNode *top;
    int size;
};

struct StackNode {
    struct Token *data;
    struct StackNode *next;
};

struct Stack *createStack();

void destroyStack(struct Stack *s, bool removeChildren);

struct StackNode *stack_peek(struct Stack *s);

void stack_push(struct Stack *s, struct Token *x);

struct Token *stack_pop(struct Stack *s);

void unloadAll(struct Stack *s, struct LinkedList *postfixList, bool stopBracket);

void printStack(struct Stack *s);


