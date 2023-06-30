#include "stdlib.h"
#include "stdio.h"
#include "queue.h"



struct Queue * createQueue(int needleSize){
    struct Queue * newQueue = malloc(sizeof(struct Queue));
    newQueue->size = needleSize;
    newQueue->front = -1;
    newQueue->rear = -1;
    unsigned char * data = malloc(sizeof(unsigned char) * needleSize);
    newQueue->data = data;
    if(data == NULL){
        free(newQueue);
        printf("something went wrong");
        return NULL;
    }
    return newQueue;
}

void destroy(struct Queue * q){
    free(q->data);
    free(q);
}



void checkBySymbols(struct Queue *q, const char needle[], long long startIndex){

    int i = 0;
    int queueIndex = q->front;
    while (needle[i] != '\0') {

        if((unsigned char)needle[i] != q->data[queueIndex]){
            printf(" %lld", startIndex);
            break;
        }
        printf(" %lld", startIndex);
        if(i+1 == q->size) break;
        startIndex++;
        i++;
        queueIndex++;
        if(queueIndex == q->size) queueIndex = 0;
    }
}

