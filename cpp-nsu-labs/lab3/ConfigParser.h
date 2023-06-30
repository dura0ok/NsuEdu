#pragma once
#include <string>
#include <memory>
#include <fstream>
#include "converters/Converter.h"

using ParsedLines = std::vector<std::vector<std::string>>;
const inline char FILE_SIGN = '$';

class ConfigParser {
 public:
  explicit ConfigParser(const std::string &name);
  explicit ConfigParser(std::ifstream &configFile);
  [[nodiscard]] ParsedLines parseConfig(std::vector<std::string> files);
 private:
  std::ifstream file;
};
