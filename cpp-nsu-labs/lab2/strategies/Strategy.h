#pragma once
#include <string>
#include <vector>
#include "../Context.h"
#include "Types.h"

namespace Strategies {
  
  class Strategy {
	public:
	  [[nodiscard]] virtual std::string getStrategyName() const = 0;
	  [[nodiscard]] virtual std::string getStrategyDescription() const = 0;
	  virtual MoveType generateMove(const Context &context) = 0;
	  virtual ~Strategy() = default;
  };
	
}