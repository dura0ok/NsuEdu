#include <stdio.h>
#include <unistd.h>

void foo(){
    printf("kek\n");
    char tmp_buf[8096];
    sleep(2);
    foo();
}

int main(){
     printf("PID %d\n", getpid());
     sleep(2);
     foo();
}
