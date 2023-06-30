#include <memory>
#include <map>
#include <stdexcept>

#include "StrategyFactory.h"
#include "AlwaysRandom.h"
#include "AlwaysDefect.h"
#include "AlwaysCollude.h"
#include "Grudger.h"
#include "CopyCat.h"
#include "Detective.h"
#include "Strateg.h"

namespace Strategies {
  
  StrategyFactory::StrategyFactory() {
	  registerStrategies();
  }
  
  void StrategyFactory::registerStrategy(std::string_view name, std::function<std::unique_ptr<Strategy>()> creator) {
	  factories[name] = std::move(creator);
  }
  
  std::unique_ptr<Strategy> StrategyFactory::create(std::string_view strategyName) {
	  auto it = factories.find(strategyName);
	  if (it != factories.end()) {
		  return it->second();
	  }
	  throw std::runtime_error("cannot create object: unknown class name: " + std::string(strategyName));
  }
  void StrategyFactory::registerStrategies() {
	  registerStrategy("always_defect", []() {
		return std::make_unique<AlwaysDefect>();
	  });
	  
	  registerStrategy("always_collude", []() {
		return std::make_unique<AlwaysCollude>();
	  });
	  
	  registerStrategy("random", []() {
		return std::make_unique<AlwaysRandom>();
	  });
	  
	  registerStrategy("grudger", []() {
		return std::make_unique<Grudger>();
	  });
	  
	  registerStrategy("copycat", []() {
		return std::make_unique<CopyCat>();
	  });
	  
	  registerStrategy("detective", []() {
		return std::make_unique<Detective>();
	  });
	  
	  registerStrategy("strateg", []() {
		return std::make_unique<Strateg>();
	  });
  }
  
  std::string StrategyFactory::getDescription(const std::string_view &strategyName) {
	  return create(strategyName)->getStrategyDescription();
  }
  std::map<std::string_view, std::string> StrategyFactory::getElementsInfo() {
	  auto res = std::map<std::string_view, std::string>();
	  for (auto const &key : factories) {
		  res.try_emplace(key.first, getDescription(key.first));
	  }
	  return res;
  }
	
}