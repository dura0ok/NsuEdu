#include <iostream>
#include "Utility.h"


void printWhat(const std::exception &e) {
  std::cerr << e.what() << '\n';
  try {
	std::rethrow_if_nested(e);
  }
  catch (const std::exception &nested) {
	std::cerr << "--- ";
	printWhat(nested);
  }
}
