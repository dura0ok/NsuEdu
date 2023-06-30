#include <stdlib.h>
#include <assert.h>
#include "operator.h"
#include "stdio.h"
#include "data_structures/linked_list.h"
#include "data_structures/stack.h"
#include "string.h"
#include "math.h"

int addition(int a, int b) {
    return a + b;
}

int substraction(int a, int b) {
    return a - b;
}

int multiplication(int a, int b) {
    return a * b;
}

int division(int a, int b) {
    return a / b;
}

int power(int num, int degree){
    return pow(num, degree);
}

struct Token *calculate(struct LinkedList *postfixList) {
    bool foundBracket = false;
    bool divisionByZero = false;

    struct Stack *operandStack = createStack();
    struct listNode *current = postfixList->head;
    struct LinkedList *ownerPointers = createLinkedList();
    for (; current != NULL; current = current->next) {

        if (current->data->type == OPERAND) {
            stack_push(operandStack, current->data);
            continue;
        }
        if (current->data->type == OPERATOR) {
            struct Token *operand2 = stack_pop(operandStack);
            struct Token *operand1 = stack_pop(operandStack);
            if (operand1 == NULL || operand2 == NULL) {
                destroyList(ownerPointers);
                destroyStack(operandStack, false);
                return NULL;
            }

            if(operand2->operand == 0 && current->data->operator == '/'){
                divisionByZero = true;
                break;
            }
            int result = current->data->func(operand1->operand, operand2->operand);

            struct Token *ownerToken = createOperandToken(result);
            listPush(ownerPointers, ownerToken);
            stack_push(operandStack, ownerToken);
            continue;
        }
        foundBracket = true;
    }

    struct Token *popToken = stack_pop(operandStack);

    destroyStack(operandStack, false);
    if(popToken == NULL && !divisionByZero){
        destroyList(ownerPointers);
        return NULL;
    }

    struct Token *exitToken = allocateToken();
    if(popToken != NULL) exitToken->operand = popToken->operand;

    destroyList(ownerPointers);

    if(divisionByZero){
        exitToken->error = DIVISION_BY_ZERO;
        return exitToken;
    }


    if(foundBracket){
        exitToken->error = SYNTAX_ERROR;
        return exitToken;
    }


    exitToken->type = OPERAND;
    exitToken->error = NO_ERRORS;

    return exitToken;
}


