#pragma once
#include <random>
#include "Strategy.h"
#include "../helpers/RandomHelper.h"

namespace Strategies {
  
  class AlwaysRandom : public Strategy {
	public:
	  MoveType generateMove([[maybe_unused]]const Context &context) override {
		  
		  if (Helpers::generateRandomInRange(0, 1)) {
			  return DEFECT;
		  }
		  return COLLUDE;
	  }
	  
	  [[nodiscard]] std::string getStrategyName() const override {
		  return "random";
	  }
	  
	  [[nodiscard]] std::string getStrategyDescription() const override {
		  return "50% return D, 50% return C(random)";
	  }
  };
	
}

