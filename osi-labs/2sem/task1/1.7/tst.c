#include <stdio.h>
#include <assert.h>
#include "coroutine.h"

int hello_world(coro_t *coro) {    
    puts("Hello");
    coro_yield(coro, 1);
    puts("World");
    return 2;
}

int main() {
    coro_t *coro = coro_new(hello_world);
    assert(coro_resume(coro) == 1);   
    assert(coro_resume(coro) == 2);    
    assert(coro_resume(coro) == -1);  
    coro_free(coro);
    return EXIT_SUCCESS;
}
