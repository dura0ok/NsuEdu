#pragma once
#include <random>
#include "../GameInfo.h"

namespace Helpers {
  int generateRandomInRange(int a, int b) {
	  std::random_device rd;
	  std::uniform_int_distribution<int> gen(a, b);
	  return gen(rd);
  }
	
}