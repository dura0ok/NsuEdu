#pragma once
#include "Strategy.h"

namespace Strategies {
  
  class AlwaysDefect : public Strategy {
	  MoveType generateMove([[maybe_unused]] const Context &context) override {
		  return DEFECT;
	  }
	  
	  [[nodiscard]] std::string getStrategyName() const override {
		  return "always_defect";
	  }
	  
	  [[nodiscard]] std::string getStrategyDescription() const override {
		  return "always return D";
	  }
  };
	
}

