#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#define NUM_THREADS 5

void *mythread(void *arg) {
  printf("mythread [%d %d %d]: Hello from mythread!\n", getpid(), getppid(), getpid());
  return NULL;
}

int main() {
  pthread_t tid[NUM_THREADS];
  int err;
  int i;

  printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), getpid());

  for (i = 0; i < NUM_THREADS; i++) {
    err = pthread_create(&tid[i], NULL, mythread, NULL);
    if (err) {
      printf("main: pthread_create() failed: %s\n", strerror(err));
      return -1;
    }
  }

  for (i = 0; i < NUM_THREADS; i++) {
    pthread_join(tid[i], NULL);
  }

  return 0;
}