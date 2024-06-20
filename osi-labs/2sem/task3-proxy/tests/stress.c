#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define URL_COUNT 4
#define NUM_COPIES 5
#define URL_MAX_LENGTH 50
#define COMMAND_LENGTH 200

char *urls[URL_COUNT] = {
        "http://212.183.159.230/30MB.zip",
        "http://212.183.159.230/301adsdasdMB.zip",
        "http://212.183.159.230/30MB.zip",
        "http://fit.ippolitov.me/"
};

char **urlsCopied;
int urlsCopiedLength = 0;

char **copyElements(char *input[], int numCopies) {
    char **copied = malloc(URL_COUNT * numCopies * sizeof(char *));
    if (copied == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < URL_COUNT * numCopies; ++i) {
        copied[i] = malloc(URL_MAX_LENGTH + 1);
        if (copied[i] == NULL) {
            perror("Failed to allocate memory");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < numCopies; ++i) {
        for (int j = 0; j < URL_COUNT; ++j) {
            strcpy(copied[urlsCopiedLength], input[j]);
            urlsCopiedLength++;
        }
    }
    return copied;
}

void shuffle(char *array[], int size) {
    srand(time(NULL));
    int i;
    for (i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        char *temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

void *runCommand(void *arg) {
    char *url = (char *) arg;
    char command[COMMAND_LENGTH];
    sprintf(command, "curl -x 127.0.0.1:8080 -o output -v %s", url);
    system(command);
    return NULL;
}

int main() {
    urlsCopied = copyElements(urls, NUM_COPIES);
    shuffle(urlsCopied, URL_COUNT * NUM_COPIES);

    pthread_t threads[URL_COUNT * NUM_COPIES];
    int i;
    for (i = 0; i < URL_COUNT * NUM_COPIES; ++i) {
        pthread_create(&threads[i], NULL, runCommand, urlsCopied[i]);
    }
    for (i = 0; i < URL_COUNT * NUM_COPIES; ++i) {
        pthread_join(threads[i], NULL);
    }

    // Free allocated memory
    for (i = 0; i < URL_COUNT * NUM_COPIES; ++i) {
        free(urlsCopied[i]);
    }
    free(urlsCopied);

    printf("All commands completed.\n");

    return 0;
}
