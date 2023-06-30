#include <ctype.h>
#include "token.h"
#include "operator.h"
#include "stdlib.h"
#include "stdio.h"

static struct Token operators[] = {
        {.type=OPERATOR, .operator='+', .func=&addition, .priority=1, .associativity=LEFT},
        {.type=OPERATOR, .operator='-', .func=&substraction, .priority=1, .associativity=LEFT},
        {.type=OPERATOR, .operator='*', .func=&multiplication, .priority=2, .associativity=LEFT},
        {.type=OPERATOR, .operator='/', .func=&division, .priority=2, .associativity=LEFT},
        {.type=OPERATOR, .operator='^', .func=&power, .priority=3, .associativity=RIGHT}
};

static struct Token brackets[] = {
        {.type = BRACKET, .bracketType=LEFT_BRACKET, .bracket='('},
        {.type = BRACKET, .bracketType=RIGHT_BRACKET, .bracket=')'}
};

struct Token *allocateToken() {
    struct Token *newToken = malloc(sizeof(struct Token));
    return newToken;
}

struct Token *createOperandToken(int number) {
    struct Token *newToken = allocateToken();
    if (newToken == NULL) return NULL;
    newToken->type = OPERAND;
    newToken->operand = number;
    return newToken;
}

struct Token *findOperator(char c) {
    size_t operatorsSize = sizeof(operators) / sizeof(struct Token);
    for (size_t i = 0; i < operatorsSize; ++i) {
        if (c == operators[i].operator) return &operators[i];
    }
    return NULL;
}

struct Token *findBracket(char c) {
    size_t bracketsSize = sizeof(brackets) / sizeof(struct Token);
    for (size_t i = 0; i < bracketsSize; ++i) {
        if (c == brackets[i].bracket) return &brackets[i];
    }
    return NULL;
}

bool isLeftAssociative(struct Token *checkToken){
    return checkToken->associativity == LEFT;
}
bool isRightAssociative(struct Token *checkToken){
    return checkToken->associativity == RIGHT;
}


