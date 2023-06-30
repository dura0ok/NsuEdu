#pragma once
#include "Strategy.h"

namespace Strategies {
  
  class CopyCat : public Strategy {
	  MoveType generateMove(const Context &context) override {
		  if (context.getMoves().empty()) {
			  return DEFECT;
		  }
		  
		  return static_cast<MoveType>(context.getMoves().at(context.getMoves().size() - 1));
	  }
	  
	  [[nodiscard]] std::string getStrategyName() const override {
		  return "copycat";
	  }
	  
	  [[nodiscard]] std::string getStrategyDescription() const override {
		  return "if strategy makes first move return D else copy previous move";
	  }
	  
  };
	
}

