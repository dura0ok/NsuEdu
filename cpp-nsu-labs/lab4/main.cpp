#include <iostream>
#include <tuple>
#include "CSVParser.h"
#include "Utility.h"

using namespace CSV;

int main() {
  try {
	CSVParser<std::string, int> parser("in.csv", 0, true, '\n', ';', '\\');
	auto names = parser.getNames();
	std::cout << std::get<0>(names) << " " << std::get<1>(names) << "\n";
	for (const auto &rs : parser) {
	  std::cout << rs;
	}
  }
  catch (const std::exception &e) {
	printWhat(e);
	return 1;
  }

  return 0;
}