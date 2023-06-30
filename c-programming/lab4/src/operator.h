#pragma once

#include "token.h"
#include "data_structures/linked_list.h"
#include "math.h"


int addition(int a, int b);

int substraction(int a, int b);

int multiplication(int a, int b);

int division(int a, int b);

int power(int num, int degree);
struct Token *calculate(struct LinkedList *postfixList);


