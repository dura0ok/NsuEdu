#pragma once
#include <map>
#include <functional>
#include "Strategy.h"

namespace Strategies {
  
  using FactoriesMap = std::map<std::string_view, std::function<std::unique_ptr<Strategy>()>>;
  
  class StrategyFactory {
	public:
	  StrategyFactory();
	  [[nodiscard]] std::unique_ptr<Strategy> create(std::string_view strategyName);
	  [[nodiscard]] std::map<std::string_view, std::string> getElementsInfo();
	  [[nodiscard]] std::string getDescription(const std::string_view &strategyName);
	private:
	  void registerStrategy(std::string_view name, std::function<std::unique_ptr<Strategy>()> creator);
	  void registerStrategies();
	  FactoriesMap factories;
  };
	
}
