#include <memory>
#include <stdexcept>
#include "ConverterFactory.h"
#include "Mute.h"
#include "Mix.h"
#include "Echo.h"

namespace Converters {

std::vector<std::string> slicingFirst(std::vector<std::string> const &v) {
  auto first = v.begin() + 1;
  auto last = v.end();

  std::vector<std::string> vector(first, last);
  return vector;
}

ConverterFactory::ConverterFactory() {
  massiveRegisterConverters();
}

void ConverterFactory::registerConverter(std::string_view name, std::function<std::unique_ptr<Converter>()> creator) {
  factories[name] = std::move(creator);
}

std::unique_ptr<Converter> ConverterFactory::create(std::string_view strategyName,
													const std::vector<std::string> &args) const {
  auto it = factories.find(strategyName);
  if (it != factories.end()) {
	auto res = it->second();
	res->setConfig(args);
	return res;
  }
  throw InvalidInput("cannot create object: unknown class name: " + std::string(strategyName));
}

void ConverterFactory::massiveRegisterConverters() {
  registerConverter("mute", []() {
	return std::make_unique<Mute>();
  });

  registerConverter("mix", []() {
	return std::make_unique<Mix>();
  });

  registerConverter("echo", []() {
	return std::make_unique<Echo>();
  });
}

std::map<std::string_view, std::string> ConverterFactory::getElementsInfo() const {
  auto res = std::map<std::string_view, std::string>();
  for (const auto &key : factories) {
	res.try_emplace(key.first, getDescription(key.first));
  }
  return res;
}

std::string_view ConverterFactory::getDescription(const std::string_view &converterName) const {
  return create(converterName)->getConverterDescription();
}

std::unique_ptr<Converter> ConverterFactory::createFromParsedLine(const std::vector<std::string> &parsedArgs) const {
  if (parsedArgs.empty()) {
	throw InvalidInput("If you need to create converter args must not be empty");
  }

  auto newConverter = create(parsedArgs[0], slicingFirst(parsedArgs));
  return newConverter;
}

ConverterFactory::ConvertersObjectList ConverterFactory::createAllConvertersFromConfig(const ParsedLines &lines) const {
  auto res = std::vector<std::unique_ptr<Converter>>();
  for (const auto &el : lines) {
	res.push_back(createFromParsedLine(el));
  }
  return res;
}

std::unique_ptr<Converter> ConverterFactory::create(std::string_view converterName) const {
  auto it = factories.find(converterName);
  if (it != factories.end()) {
	return it->second();
  }

  throw InvalidInput("cannot create object: unknown command name: " + std::string(converterName));
}

}