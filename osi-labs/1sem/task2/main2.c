#include <unistd.h>
int main(void) {
  write(1, "hello, world!\n", 14);
  return 0;
}