#include <stdio.h>
#include <pthread.h>

void* myThread(void* arg) {
  return "hello world";
}

int main() {
  pthread_t tid;
  pthread_create(&tid, NULL, myThread, NULL);
  
  void* retVal;
  pthread_join(tid, &retVal);
  printf("Returned string: %s\n", (char*)retVal);
  
  return 0;
}
