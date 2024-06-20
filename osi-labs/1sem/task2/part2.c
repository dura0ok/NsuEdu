#include <stdio.h>
#include <string.h>


/*

asm {
    mov ax,4       // (I/O Func.)
    mov bx,1       // (Output func)  
    lds cx,"Hello, world" // (address of the string)
    mov dx,6       //  (length of the string)
    int 0x21       // system call
}


*/

void my_custom_writer(char *msg, int len){
    __asm__ __volatile__(
        "int $0x80"
        :: "a"(4),
        "b"(1),
        "c"(msg),
        "d"(len)
);
}

int main(){
    char *msg = "Hello, world!";
    int len = strlen(msg);

    
    my_custom_writer(msg, len);

    /* Exit */
    asm(    "movl $1,%eax;"
            "xorl %ebx,%ebx;"
            "int  $0x80"
    );



}
