#pragma once
#include <map>
#include <memory>
#include <vector>
#include <functional>
#include "Converter.h"
#include "../ConfigParser.h"

namespace Converters {

class ConverterFactory {
 public:
  ConverterFactory();
  [[nodiscard]] std::unique_ptr<Converter> create(std::string_view strategyName,
												  const std::vector<std::string> &args) const;

  using ConvertersObjectList = std::vector<std::unique_ptr<Converter>>;
  [[nodiscard]] ConvertersObjectList createAllConvertersFromConfig(const ParsedLines &lines) const;
  [[nodiscard]] std::map<std::string_view, std::string> getElementsInfo() const;
 private:
  using FactoriesMap = std::map<std::string_view, std::function<std::unique_ptr<Converter>()>>;
  [[nodiscard]] std::string_view getDescription(const std::string_view &converterName) const;
  void registerConverter(std::string_view name, std::function<std::unique_ptr<Converter>()> creator);
  void massiveRegisterConverters();
  [[nodiscard]] std::unique_ptr<Converter> createFromParsedLine(const std::vector<std::string> &parsedArgs) const;
  FactoriesMap factories;
  [[nodiscard]] std::unique_ptr<Converter> create(std::string_view converterName) const;
};

}
