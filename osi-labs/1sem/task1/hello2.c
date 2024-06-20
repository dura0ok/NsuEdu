#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
int main(int argc, char **argv) {
    void *handle;
    void (*hello_func)();
    char *error;
    handle = dlopen ("libmy2.so", RTLD_LAZY);
    if (!handle) {
        fputs (dlerror(), stderr);
        exit(1);
    }

    hello_func = dlsym(handle, "hello_from_static_lib");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s\n", error);
        exit(1);
    }
    (*hello_func)();
    dlclose(handle);
}