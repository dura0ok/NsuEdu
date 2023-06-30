#include <stdio.h>
#include "stdlib.h"
#include "string.h"

#define ALPHABET_SIZE 256

void prepareTable(unsigned int *table, const char *needle, unsigned int needleSize){
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        table[i] = needleSize;
    }
    for (unsigned int i = 0; i < needleSize - 1; i++) {
        table[(unsigned char)(needle[i])] = needleSize - i - 1;
    }
}

void checkBySymbols(const unsigned char *candidate, size_t k, const unsigned char needle[], long long startIndex, unsigned long needleSize){
    size_t counter = k;
    for (int i = (int)needleSize - 1; i >= 0; i--) {
        printf("%lld ", startIndex);
        if(needle[i] != candidate[counter]) break;
        counter--;
        startIndex--;
    }
}

int main(void) {
    char needle[18];
    if (!fgets(needle, 18, stdin)) return 0;
    needle[strcspn(needle, "\n")] = 0;
    unsigned int needleSize = strlen(needle);
    char *candidate = malloc((needleSize + 1) * sizeof(char));
    candidate[needleSize] = '\0';
    unsigned int skipLimit = needleSize;
    unsigned int skipCounter = 1;
    unsigned int globalCounter = 1;

    unsigned int *table = malloc(ALPHABET_SIZE * sizeof(unsigned int));
    prepareTable(table, needle, needleSize);


    unsigned char buffer[BUFSIZ];
    while (1) {
        size_t n = fread(buffer, 1, BUFSIZ, stdin);
        if (n == 0) break;
        for (size_t k = 0; k < n; k++) {
            unsigned char c = buffer[k];

            if(skipCounter == skipLimit && skipCounter != 0){
                checkBySymbols((unsigned char *) &buffer, k, (const unsigned char*)needle, globalCounter, needleSize);
                skipCounter = 1;
                skipLimit = table[c];
            } else{
                skipCounter++;
            }

            globalCounter++;
            if(k < needleSize - 1)continue;
        }
    }
    free(table);
    free(candidate);
    return 0;
}
