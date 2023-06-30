#pragma once


#include <stdbool.h>

enum Type {
    OPERAND,
    OPERATOR,
    BRACKET,
};

enum BracketType {
    LEFT_BRACKET,
    RIGHT_BRACKET
};

enum Associativity{
    LEFT,
    RIGHT
};

enum Error {
    NO_ERRORS,
    SYNTAX_ERROR,
    DIVISION_BY_ZERO
};

struct Token {
    enum Type type;
    enum Associativity associativity;
    enum Error error;

    char operator;
    int priority;
    int (*func)(int, int);

    int operand;

    enum BracketType bracketType;
    char bracket;
};

struct Token *createOperandToken(int number);

struct Token *findOperator(char c);

struct Token *findBracket(char c);

struct Token *allocateToken();

bool isLeftAssociative(struct Token *checkToken);
bool isRightAssociative(struct Token *checkToken);


