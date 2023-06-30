#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define NUMBER_SIZE 10

void swap(char* num, int i, int j){
    char temp = num[i];
    num[i] = num[j];
    num[j] = temp;
}

int find_max_index(const char *num, size_t num_size){
    for (int i = (int)num_size - 2; i >= 0; i--) {
        if (num[i] < num[i + 1]) {
            return i;
        }
    }
    return -1;
}

int find_index_bigger_element(const char *num, size_t num_size, int element){
    for (int i = (int)num_size - 1; i >= 0; i--) {
        if (num[i] > element) {
            return i;
        }
    }
    return -1;
}

void reverse_sequence_part(int begin, char* num, size_t num_size){
    int end = (int)num_size - 1;

    while (begin < end)
    {
        swap(num, begin, end);
        begin++;
        end--;
    }
}

void narajana(unsigned int permutation_counter, char *num, size_t num_size){
    int index = (int)find_max_index(num, num_size);
    for (size_t i = 0; i < permutation_counter; i++) {
        if(index == -1) break;
        int element = (int)num[index];
        int swapIndex = (int)find_index_bigger_element(num, num_size, element);
        swap(num, index, swapIndex);
        reverse_sequence_part(index+1, num, num_size);
        printf("%s\n", num);
        index = (int)find_max_index(num, num_size);
    }
}

void bad_input(){
    printf("bad input");
}


bool validate_num(const char *num, size_t num_size) {
    int digits_has[10] = {0};
    for (size_t i = 0; i < num_size; ++i) {
        char c = num[i];
        if(!isdigit(c)) return false;
        if(digits_has[c - '0'] > 0) return false;
        digits_has[c - '0']++;
    }
    return true;
}


int main(){
    char num[BUFSIZ];
    if(fgets(num, BUFSIZ, stdin) == NULL){
        bad_input();
        return 0;
    }
    num[strcspn(num, "\n")] = 0;

    size_t num_size = strlen(num);

    if(!validate_num(num, num_size)){
        bad_input();
        return 0;
    }





    unsigned int permutation_counter;
    if(scanf("%u", &permutation_counter) != 1){
        bad_input();
        return 0;
    }
    narajana(permutation_counter, num, num_size);

}


