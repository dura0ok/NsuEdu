#pragma once
#include <tuple>
#include <string>
#include <memory>
#include "gameHistory/FileStorage.h"
#include "strategies/Types.h"

namespace Strategies {
  
  class Context {
	public:
	  Context(const Strategies::MovesVector &moves, const Strategies::Rivals &names,
			  std::shared_ptr<PrisonerConfig::Storage> storage = nullptr);
	  [[nodiscard]] const Rivals &getNames() const;
	  [[nodiscard]] const MovesVector &getMoves() const;
	  [[nodiscard]] const std::shared_ptr<PrisonerConfig::Storage> &getStorage() const;
	private:
	  const Strategies::Rivals &names;
	  const Strategies::MovesVector &moves;
	  std::shared_ptr<PrisonerConfig::Storage> storage;
  };
}
