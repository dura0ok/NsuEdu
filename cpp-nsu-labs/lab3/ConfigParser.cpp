#include <regex>
#include "converters/ConverterFactory.h"
#include "ConfigParser.h"
#include "helpers/LineHelper.h"
#include "CustomExceptions.h"

const inline char COMMENT_SIGN = '#';


std::vector<std::string> splitStringByMatches(const std::string &s, const std::regex &re) {
  std::vector<std::string> elems;

  for (auto it = std::sregex_iterator(s.begin(), s.end(), re); it != std::sregex_iterator(); it++) {
	const std::smatch &match = *it;
	elems.push_back(match.str(0));
  }

  return elems;
}

ConfigParser::ConfigParser(const std::string &name) {
  this->file.open(name);
  if (!this->file.is_open()) {
	throw IOException("Could not open file: " + name);
  }
}

ParsedLines ConfigParser::parseConfig(std::vector<std::string> files) {
  auto converterFactory = Converters::ConverterFactory();
  auto res = ParsedLines{};

  std::string line;
  std::regex re(R"([a-zA-Z0-9_.-\\$]+)");

  while (getline(file, line)) {
	if (line.empty()) {
	  continue;
	}

	if (Helpers::trim(line).find(COMMENT_SIGN) == 0) {
	  continue;
	}

	auto tokens = splitStringByMatches(line, re);
	if (tokens.empty()) {
	  continue;
	}

	for (auto &token : tokens) {
	  if (Helpers::trim(token).find(FILE_SIGN)==0) {
		auto index = std::stoi(Helpers::trim(token).erase(0, 1));
		if (static_cast<size_t>(index) >= files.size() || index <= 0) {
		  throw InvalidInput(
			  "Index: {" + std::to_string(index) + "} does not exist in the files which you transferred.");
		}
		token = FILE_SIGN + files.at(index);
	  }
	}

	res.push_back(tokens);
  }

  return res;
}

ConfigParser::ConfigParser(std::ifstream &configFile) : file(std::move(configFile)) {
}
