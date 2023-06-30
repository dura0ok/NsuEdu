#include <stdio.h>
#include "ctype.h"
#include "stdbool.h"
#include <string.h>
#include "data_structures/stack.h"
#include "stdlib.h"
#include "limits.h"
#include "data_structures/linked_list.h"
#include "operator.h"

enum inputAnswer{
    OK,
    ERROR,
    CANCEL
};

void badInput() {
    printf("syntax error");
}


void freeData(struct Stack *operatorStack, struct LinkedList *postfixList){
    destroyList(postfixList);
    destroyStack(operatorStack, true);
}


static int buildNumber(char *num, bool *foundDigit, unsigned int *currentNumIndex) {
    num[(*currentNumIndex)] = '\0';
    int number;
    if(sscanf(num, "%d", &number) != 1){
        badInput();
    }
    (*foundDigit) = false;
    (*currentNumIndex) = 0;
    return number;
}


static void handleDigit(char *num, char c, bool *foundDigit, unsigned int *currentNumIndex) {
    num[(*currentNumIndex)] = c;
    (*foundDigit) = true;
    (*currentNumIndex)++;
}

static void handleOperator(struct Stack *operatorStack, struct LinkedList *outList, struct Token *currentToken) {
    struct StackNode *stackTopNode = stack_peek(operatorStack);
    if (stackTopNode == NULL || currentToken->priority > stackTopNode->data->priority) {
        stack_push(operatorStack, currentToken);
        return;
    }

    while ((stackTopNode != NULL) &&
    ((isLeftAssociative(currentToken) && currentToken->priority <= stackTopNode->data->priority) ||
    (isRightAssociative(currentToken) && currentToken->priority < stackTopNode->data->priority))){
        struct Token *outToken;
        outToken = stack_pop(operatorStack);
        listPush(outList, outToken);
        stackTopNode = stack_peek(operatorStack);
    }
    stack_push(operatorStack, currentToken);
}



static void handleBracket(struct Stack *operatorStack, struct LinkedList *outList, struct Token *currentBracket) {
    if (currentBracket->bracketType == LEFT_BRACKET) {
        stack_push(operatorStack, currentBracket);
        return;
    }
    unloadAll(operatorStack, outList, true);
    stack_pop(operatorStack);
}

bool checkChar(char currentChar, char prevChar){
    if(prevChar == '\0') return true;
    if(currentChar == '('){
        return findOperator(prevChar) != NULL|| findBracket(prevChar) != NULL;
    }
    if(isdigit(currentChar)){
        return findOperator(prevChar) != NULL|| prevChar == '(' || isdigit(prevChar);
    }
    return true;
}

int handleInput(struct Stack *operatorStack, struct LinkedList *postfixList, int lastResult){
    char buffer[BUFSIZ];
    char num[CHAR_MAX] = {'\0'};
    char prevChar = '\0';
    bool foundNewLine = false;
    bool foundDigit = false;
    unsigned int currentNumIndex = 0;

    while(fgets(buffer, BUFSIZ , stdin) != NULL) {
        if(strcmp(buffer, "exit\n") == 0){
            return CANCEL;
        }
        for (size_t i = 0; buffer[i] != 0; ++i) {
            char c = buffer[i];
            if(buffer[i] == '\n'){
                foundNewLine = true;
                break;
            }
            if(!checkChar(c, prevChar)){
                return ERROR;
            }

            prevChar = c;

            struct Token *currentBracket = findBracket(c);
            if (currentBracket != NULL) {
                if (foundDigit) listPush(postfixList, createOperandToken(buildNumber(num, &foundDigit, &currentNumIndex)));
                if(currentBracket->bracketType == LEFT_BRACKET){
                    stack_push(operatorStack, currentBracket);
                    continue;
                }
                handleBracket(operatorStack, postfixList, currentBracket);
                continue;
            }

            struct Token *currentOperator = findOperator(c);

            if (currentOperator != NULL) {
                if (foundDigit){
                    listPush(postfixList, createOperandToken(buildNumber(num, &foundDigit, &currentNumIndex)));
                }
                handleOperator(operatorStack, postfixList, currentOperator);
                continue;
            }

            if (isdigit(c)) {
                handleDigit(num, c, &foundDigit, &currentNumIndex);
            }else{
                if(c == '$'){
                    listPush(postfixList, createOperandToken(lastResult));
                    continue;
                }
                return ERROR;
            }

        }
        if (foundDigit) listPush(postfixList, createOperandToken(buildNumber(num, &foundDigit, &currentNumIndex)));
        if(foundNewLine) break;
    }
    unloadAll(operatorStack, postfixList, false);
    return OK;
}


int main(int argc, char * argv[]) {
    struct Stack *operatorStack = createStack();
    struct LinkedList *postfixList = createLinkedList();
    int lastResult = 0;
    if(postfixList == NULL) return 0;
    bool isInteractive = false;
    if(argc > 1 && strcmp(argv[1], "-i") == 0){
        isInteractive = true;
    }


    while(1){
        int error = handleInput(operatorStack, postfixList, lastResult);
        if(error == ERROR){
            badInput();
            if(!isInteractive) break;
            printf("\n");
            continue;
        }

        if(error == CANCEL){
            printf("exiting");
            return 0;
        }

        struct Token *result = calculate(postfixList);

        if(result == NULL || result->error == SYNTAX_ERROR){
            badInput();
        }
        else if(result->error == DIVISION_BY_ZERO){
            printf("division by zero\n");
        }else{
            lastResult = result->operand;
            printf("%d\n", result->operand);
        }


        free(result);
        if(!isInteractive) break;
    }

    freeData(operatorStack, postfixList);

    return 0;
}
