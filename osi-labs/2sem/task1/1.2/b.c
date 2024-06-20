
#include <stdio.h>
#include <pthread.h>

void* myThread(void* arg) {
  return (void*)42;
}

int main() {
  pthread_t tid;
  pthread_create(&tid, NULL, myThread, NULL);
  
  long retVal;
  pthread_join(tid, (void**) &retVal);
  printf("Returned value: %ld\n", (long)retVal);
  
  return 0;
}
