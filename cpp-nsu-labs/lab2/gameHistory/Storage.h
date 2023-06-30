#pragma once
#include <iostream>
#include <vector>
#include <array>
#include "../strategies/Types.h"

namespace PrisonerConfig {
  using Names = std::array<std::string, 3>;
  using StorageElementData = std::vector<std::vector<Strategies::MoveType>>;
  
  class Storage {
	public:
	  virtual StorageElementData loadData(const std::string &firstRival, const std::string &secondRival) = 0;
	  [[nodiscard]] virtual bool isStorageOK() const = 0;
	  virtual void writeGame(const Names &strategyNames, const std::vector<Strategies::MoveType> &moves) = 0;
	  virtual ~Storage() = default;
  };
}

