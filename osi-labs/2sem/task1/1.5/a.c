#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>

void* block_signals(void* arg) {
    sigset_t mask;
    sigfillset(&mask);
    pthread_sigmask(SIG_BLOCK, &mask, NULL);

    while (1) {
        char* message = "asd\n";
        write(STDOUT_FILENO, message, strlen(message));

        // This thread will block all signals
    }
}

void handle_sigint(int signum) {
    char* message = "Received SIGINT signal\n";
    write(STDOUT_FILENO, message, strlen(message));
    exit(1);
    // Handle SIGINT signal
}

void handle_sigquit(int signum) {
    char* message = "Received SIGQUIT signal\n";
    write(STDOUT_FILENO, message, strlen(message));
    // Handle SIGQUIT signal
}

void* receive_sigint(void* arg) {
    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    while (1) {
        // This thread will receive SIGINT signal using signal handler
    }
}

void* receive_sigquit(void* arg) {
    struct sigaction sa;
    sa.sa_handler = handle_sigquit;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGQUIT, &sa, NULL);

    while (1) {
        // This thread will receive SIGQUIT signal using signal handler
    }
}

int main() {
    pthread_t thread1, thread2, thread3;

    pthread_create(&thread1, NULL, block_signals, NULL);
    pthread_create(&thread2, NULL, receive_sigint, NULL);
    pthread_create(&thread3, NULL, receive_sigquit, NULL);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);

    return 0;
}