#include "stdlib.h"
#include "stack.h"
#include "stdio.h"

struct Stack *createStack() {
    struct Stack *newStack = malloc(sizeof(struct Stack));
    newStack->size = 0;
    newStack->top = NULL;
    return newStack;
}

struct StackNode *stack_peek(struct Stack *s) {
    if (s == NULL) return NULL;
    return s->top;
}


void stack_push(struct Stack *s, struct Token *x) {
    if (s == NULL) return;
    struct StackNode *node = malloc(sizeof(struct StackNode));
    if (node == NULL) return;
    node->data = x;
    s->size++;
    if (s->top == NULL) {
        s->top = node;
        s->top->next = NULL;
        return;
    }

    node->next = s->top;
    s->top = node;
}

struct Token *stack_pop(struct Stack *s) {
    if (s == NULL) return NULL;
    struct StackNode *exitNode = s->top;
    if (exitNode == NULL) return NULL;
    struct Token *exitData = s->top->data;
    s->top = s->top->next;
    s->size--;
    free(exitNode);
    return exitData;

}

void unloadAll(struct Stack *s, struct LinkedList *outStack, bool stopBracket) {
    struct StackNode *stackTop = s->top;

    while (stackTop != NULL) {
        if (stopBracket && stackTop->data->type == BRACKET && stackTop->data->bracketType == LEFT_BRACKET) return;
        stackTop = stack_peek(s)->next;
        listPush(outStack, stack_pop(s));
    }
}


void printStack(struct Stack *s) {
    struct StackNode *stackTop = s->top;
    if (stackTop == NULL) return;
    while (stackTop != NULL) {
        if (stackTop->data->type == OPERATOR) {
            printf("%c ", stackTop->data->operator);
        } else {
            printf("%d ", stackTop->data->operand);
        }
        stackTop = stackTop->next;
    }
}

void destroyStack(struct Stack *s, bool removeChildren) {
    if (s == NULL) return;
    if (s->top == NULL) {
        free(s);
        return;
    }

    if (removeChildren) {
        struct StackNode *current = s->top;
        while (1) {
            if (current == NULL) break;
            struct StackNode *tmp = current;
            current = current->next;
            free(tmp);
        }
    }
    free(s);
}


