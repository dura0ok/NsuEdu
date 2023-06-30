#pragma once
#include "Strategy.h"

namespace Strategies {
  
  class AlwaysCollude : public Strategy {
	  MoveType generateMove([[maybe_unused]] const Context &context) override {
		  return COLLUDE;
	  }
	  
	  [[nodiscard]] std::string getStrategyName() const override {
		  return "always_collude";
	  }
	  
	  [[nodiscard]] std::string getStrategyDescription() const override {
		  return "always return C";
	  }
  };
	
}

