#include "Context.h"
#include <utility>

namespace Strategies {
  
  Strategies::Context::Context(const Strategies::MovesVector &moves, const Strategies::Rivals &names,
							   std::shared_ptr<PrisonerConfig::Storage> storage)
	  : names(names), moves(moves), storage(std::move(storage)) {
	  
  }
  const Strategies::Rivals &Strategies::Context::getNames() const {
	  return names;
  }
  const Strategies::MovesVector &Strategies::Context::getMoves() const {
	  return moves;
  }
  const std::shared_ptr<PrisonerConfig::Storage> &Strategies::Context::getStorage() const {
	  return storage;
  }
	
}