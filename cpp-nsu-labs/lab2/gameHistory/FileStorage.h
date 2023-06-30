#pragma once
#include <map>
#include <string>
#include <vector>
#include "../strategies/Types.h"
#include "Storage.h"

namespace PrisonerConfig {
  
  class FileStorage : public Storage {
	public:
	  explicit FileStorage(const std::string &storagePath);
	  [[nodiscard]] bool isStorageOK() const override;
	  void writeGame(const Names &strategyNames, const std::vector<Strategies::MoveType> &moves) override;
	  [[nodiscard]] StorageElementData loadData(const std::string &firstRival, const std::string &secondRival) override;
	private:
	  std::shared_ptr<std::fstream> dataFile;
  };
	
}