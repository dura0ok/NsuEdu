#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>

#define NUM_THREADS 5

int global_var = 0;

void *mythread(void *arg) {
    printf("mythread [%d %d]: Hello from mythread!\n", getpid(), getppid());
    pthread_t tid = pthread_self();
    pid_t pid = getpid();
    pid_t tid_sys = syscall(SYS_gettid);

    printf("Thread ID from pthread_self(): %lu\n", tid);
    printf("Thread ID from getpid(): %d\n", pid);
    printf("Thread ID from syscall(SYS_gettid): %d\n", tid_sys);

    if (pthread_equal(tid, *(pthread_t *)arg)) {
        printf("pthread_self() thread ID matches pthread_create() thread ID\n");
    } else {
        printf("pthread_self() thread ID does not match pthread_create() thread ID\n");
    }

  int local_var = 123;
  static int static_var = 456;
  const int const_var = 789;
  printf("mythread: \n");
  printf("Address of local_var: %p\n", &local_var);
  printf("Address of static_var: %p\n", &static_var);
  printf("Address of const_var: %p\n", &const_var);
  printf("Address of global_var: %p\n", &global_var);

    sleep(30);
    return NULL;
}

int main() {
    pthread_t tid[NUM_THREADS];
    int err;
    int i;

    printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), getpid());

    for (i = 0; i < NUM_THREADS; i++) {
        err = pthread_create(&tid[i], NULL, mythread, (void *)&tid[i]);
        if (err) {
            printf("main: pthread_create() failed: %s\n", strerror(err));
            return -1;
        }
    }

    printf("SLEEPING %d\n", getpid());
    sleep(30);

    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(tid[i], NULL);
    }

    return 0;
}
