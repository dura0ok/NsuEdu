#!/bin/zsh
gcc -c static_lib.c -o static_lib.o
ar r libmy1.a static_lib.o
#gcc hello.c libmy1.a -o hello_with_static_lib
gcc hello.c -L. -lmy1 -o hello_with_static_lib



gcc -c -fPIC static_lib.c -o dynamic_lib.o
gcc -shared dynamic_lib.o -o libmy2.so

gcc hello.c libmy2.so -o hello_with_dynamic_lib -Wl,-rpath="\$ORIGIN"
#gcc hello.c -o hello_with_dynamic_lib -L. -lmy2

gcc hello2.c -o dlopen -Wl,-rpath="\$ORIGIN"
