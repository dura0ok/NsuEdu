#include <iostream>
#include "BigInt.h"

int main() {

  std::string s;

  NumbersLibrary::BigInt t;
  std::cin >> t >> s;
  std::cout << "Your Number: " << t << ", s=" << s << "\n";

  return 0;
}
