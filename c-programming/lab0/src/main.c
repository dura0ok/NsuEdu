#include <stdio.h>
#include "stdlib.h"
#include "ctype.h"
#include "math.h"
#include "linked_list.h"

const int precision = 13;

int charToInt(char c)
{
    if (c >= '0' && c <= '9')
        return (int)c - '0';
    else
        return (int)c - 'A' + 10;
}

char intToChar(int num){
    char symbol = (char)(((int)num) + '0');
    if(num > 9 && num  < 16){
        symbol =  (char)('A' + (num - 10));
    }
    return symbol;
}

int checkCorrectChar(char c){
    return isalpha(c) || isdigit(c) || c == '.';
}

void badInput(){
    printf("bad input");
    exit(0);
}


void decIntToBase(struct LinkedList *digits, int base, long long num) {
    while(num > 0){
        char symbol = intToChar(num % base);
        if(base <= 10 && !isdigit(symbol)){
            free(digits);
            badInput();
            return;
        }

        push(digits, symbol);
        num /= base;
    }
    reverse(digits);
    if(digits->head == NULL){
        push(digits, '0');
    }
}

void decFractionToBase(struct LinkedList *digits, int base, double fraction_part){
    if(fraction_part == 0){
        return;
    }
    push(digits, '.');
    for(int i = 0; i < precision; i++){
        if(fraction_part == 0){
            break;
        }
        fraction_part = fraction_part * base;
        int fraction_part_int = (int)fraction_part;
        char symbol = intToChar(fraction_part_int);


        push(digits, symbol);
        fraction_part = fraction_part - (int)fraction_part;
    }
}

void decToBase(struct LinkedList *digits, int base, double num) {
    long long integer_part = (long long )num;
    double fraction_part = num - integer_part;
    decIntToBase(digits, base, integer_part);
    decFractionToBase(digits, base, fraction_part);
}

int getDigitsBeforeDot(char num[14]){
    int flag = 0;
    int before = 0;
    int after = 0;
    for (int i = 0; i < 14; i++) {
        if(num[i] == '\0') break;
        if(num[i] == '.'){
            if(flag){
                badInput();
            }
            flag = 1;
            continue;
        }
        if(!flag){
            before++;
            continue;
        }
        after++;
    }
    if(before == 0 || (after == 0 && flag)){
        badInput();
    }
    return before;
}

double toDec(int from_base, char num[14]){
    double decimal_res = 0;
    int before_counter = getDigitsBeforeDot(num) - 1;
    int flag = 0;
    int after_counter = -1;


    for (int i = 0; i < 14; i++) {
        if(num[i] == '\0') break;

        int num_int = charToInt(toupper(num[i]));
        if(num_int >= from_base || !checkCorrectChar(num[i])){
            badInput();
        }

        if(num[i] == '.'){
            flag = 1;
            continue;
        }
        if(!flag){
            decimal_res += (int)(num_int) * pow(from_base, before_counter);
            before_counter--;
            continue;
        }
        decimal_res += num_int * pow(from_base, after_counter);
        after_counter--;
    }
    return decimal_res;
}

int main() {

    int b1, b2;
    if(scanf("%d %d", &b1, &b2) != 2){
        badInput();
    }
    if(b1 > 16 || b1 < 2 || b2 > 16 || b2 < 2){
        badInput();
    }

    struct LinkedList digits = {0};

    if(b1 == 10){
        double decimal_num;
        if(scanf("%lf", &decimal_num) != 1){
            badInput();
        }
        decToBase(&digits, b2, decimal_num);
        display(&digits);
        destroy(&digits);
        return 0;
    }
    char num[15] = {'\0'};
    if(scanf("%14s", num) != 1){
        badInput();
    }

    double dec = toDec(b1, num);
    if(b2 == 10){
        printf("%f", dec);
        return 0;
    }

    decToBase(&digits, b2, dec);
    display(&digits);
    destroy(&digits);
    return 0;
}
