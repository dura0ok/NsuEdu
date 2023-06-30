#pragma once
#include <random>
#include <memory>
#include "Strategy.h"
#include "CopyCat.h"
#include "AlwaysCollude.h"

namespace Strategies {
  
  class Detective : public Strategy {
	public:
	  explicit Detective()
		  : movesCount(0) {
		  copyCat = std::unique_ptr<Strategy>(new CopyCat);
		  alwaysCollude = std::unique_ptr<Strategy>(new CopyCat);
	  }
	  
	  [[nodiscard]] std::string getStrategyDescription() const override {
		  return "first moves like copy cat, 2,4,5 -> D. 3 -> C else: always collude strategy";
	  }
	  
	  MoveType generateMove(const Context &context) override {
		  auto moves = context.getMoves();
		  
		  movesCount++;
		  switch (movesCount) {
			  case 1:
			  case 3:
			  case 4:return DEFECT;
			  case 2:return COLLUDE;
			  default:
				  if (!moves.empty() && moves.back() == COLLUDE) {
					  return copyCat->generateMove(context);
				  }
				  return alwaysCollude->generateMove(context);
			  
		  }
	  }
	  
	  [[nodiscard]] std::string getStrategyName() const override {
		  return "detective";
	  }
	
	private:
	  size_t movesCount = 0;
	  std::unique_ptr<Strategy> copyCat;
	  std::unique_ptr<Strategy> alwaysCollude;
  };
	
}

