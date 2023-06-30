#include <stdio.h>
#include <string.h>
#include "math.h"
#include "queue.h"

unsigned int generateHash(const unsigned char c, unsigned long long index) {
    return (c % 3) * (int) powl(3, index);
}

unsigned int generateNeedleHash(const char c[]) {
    unsigned int result = 0;
    unsigned long length = strlen(c);
    for (unsigned long i = 0; i < length; ++i) {
        unsigned char ch = (unsigned char) c[i];
        result += generateHash(ch, i);
    }

    return result;
}

int main() {
    char needle[18];
    if(!fgets(needle, 18, stdin)) return 0;
    needle[strcspn(needle, "\n")] = 0;
    unsigned int needleHash = generateNeedleHash(needle);

    size_t needleSize = strlen(needle);
    struct Queue *q = createQueue((int) needleSize);
    unsigned char buffer[1024];

    long long i = 0;
    unsigned int hash = 0;
    int found = 0;

    const unsigned int factor = (unsigned int) pow(3, (unsigned int) needleSize - 1);

    while (1) {
        size_t n = fread(buffer, 1, 1024, stdin);
        if (n == 0) break;
        for(size_t k = 0; k < n; k++) {
            unsigned char c = buffer[k];
            if ((size_t) i <= needleSize - 1) {
                if(q->front == -1) q->front = 0;
                q->rear = (q->rear + 1) % q->size;
                q->data[q->rear] = c;
                hash += generateHash((unsigned char) c, i);
            } else {
                unsigned char exitChar = q->data[q->front];
                if (q->front == q->rear){
                    q->front = -1;
                    q->rear = -1;
                }
                else {
                    q->front = (q->front + 1) % q->size;
                }
                if(q->front == -1) q->front = 0;
                q->rear = (q->rear + 1) % q->size;
                q->data[q->rear] = c;
                hash = ((hash - (exitChar % 3)) / 3) + factor * (c % 3);
            }
            if (hash == needleHash && (size_t) i >= needleSize - 1) {
                if (found == 0) printf("%u", hash);
                long long startIndex = i - (long long) needleSize + 2;
                if (startIndex - 2 == 0) startIndex--;
                checkBySymbols(q, needle, startIndex);
                found = 1;
            }
            i++;
        }
    }


    if (found == 0) {
        printf("0");
    }

    destroy(q);
    return 0;
}
