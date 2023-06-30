#pragma once
#include "Strategy.h"

namespace Strategies {
  
  class Grudger : public Strategy {
	public:
	  
	  MoveType generateMove(const Context &context) override {
		  handlePersonCheated(context.getMoves());
		  if (isPersonAlreadyCheated) {
			  return DEFECT;
		  }
		  
		  return COLLUDE;
	  }
	  
	  [[nodiscard]] std::string getStrategyName() const override {
		  return "grudger";
	  }
	  
	  [[nodiscard]] std::string getStrategyDescription() const override {
		  return "cooperate while person not start cheating else defeat";
	  }
	private:
	  bool isPersonAlreadyCheated = false;
	  
	  void handlePersonCheated(const MovesVector &moves) {
		  if (isPersonAlreadyCheated) {
			  return;
		  }
		  if (moves.size() == 1 && moves.back() == COLLUDE) {
			  isPersonAlreadyCheated = true;
			  return;
		  }
		  
		  if (moves.size() > 1 && (moves.at(moves.size() - 1) == COLLUDE || moves.at(moves.size() - 2) == COLLUDE)) {
			  isPersonAlreadyCheated = true;
		  }
	  }
  };
	
}

