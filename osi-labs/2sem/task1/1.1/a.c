#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

void *mythread(void *arg) {
  while(1){
    printf("mythread [%d %d %d]: Hello from mythread!\n", getpid(), getppid(), getpid());
  }
  
  return NULL;
}
  
int main() {
  pthread_t tid;
  int err;

  printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), getpid());
  printf("THREAD BEFORE CREATE!!\n");
  err = pthread_create(&tid, NULL, mythread, NULL);
  printf("THREAD AFTER CREATE!!\n");
  if (err) {
      printf("main: pthread_create() failed: %s\n", strerror(err));
    return -1;
  }
  pthread_detach(tid);
  pthread_exit(NULL);

  //pthread_detach(tid);
  // int res = pthread_join(tid, NULL);
  // printf("%s", strerror(res));
  return 0;
}
