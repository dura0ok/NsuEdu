#include <random>
#include "LineHelper.h"

namespace Helpers {

std::string getWhiteSpace() {
  return " \n\r\t\f\v";
}

std::string ltrim(const std::string &s) {
  size_t start = s.find_first_not_of(getWhiteSpace());
  return (start == std::string::npos) ? "" : s.substr(start);
}

std::string rtrim(const std::string &s) {
  size_t end = s.find_last_not_of(getWhiteSpace());
  return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string trim(const std::string &s) {
  return rtrim(ltrim(s));
}
std::string generateUID() {
  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<int> dist(0, 15);

  const char *v = "0123456789abcdef";
  const int dash[] = {0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0};

  std::string res;
  for (bool i : dash) {
	if (i) {
	  res += "-";
	}
	res += v[dist(rng)];
	res += v[dist(rng)];
  }
  return res;
}

}