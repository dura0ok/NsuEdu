#pragma once
#include <fstream>
#include "Strategy.h"
#include "../gameHistory/FileStorage.h"

namespace Strategies {
  
  class Strateg : public Strategy {
	  MoveType generateMove(const Context &context) override {
		  
		  const auto &storage = context.getStorage();
		  if (storage == nullptr) {
			  throw std::runtime_error("strateg need storage. please add valid --config path to storage");
		  }
		  
		  PrisonerConfig::StorageElementData res;
		  try {
			  res = storage->loadData(std::get<0>(context.getNames()), std::get<1>(context.getNames()));
		  }
		  catch (const std::exception &e) {
			  return COLLUDE;
		  }
		  
		  if (res.empty()) {
			  return COLLUDE;
		  }
		  auto randomGameIndex = Helpers::generateRandomInRange(0, static_cast<int>(res.size()) - 1);
		  return res.at(randomGameIndex).back();
		  
	  }
	  
	  [[nodiscard]] std::string getStrategyName() const override {
		  return "strateg";
	  }
	  
	  [[nodiscard]] std::string getStrategyDescription() const override {
		  return "first move cheat or storage not found. If storage included return last move on random previos game with this opponents";
	  }
  };
	
}

