#pragma once
#include <map>
#include <tuple>
#include <fstream>
#include <vector>
#include <memory>
#include <set>
#include "strategies/Strategy.h"
#include "helpers/LineHelper.h"
#include "gameHistory/FileStorage.h"
#include "Matrix.h"

namespace PrisonerConfig {
  
  using StrategiesVector = std::vector<std::pair<std::unique_ptr<Strategies::Strategy>, int>>;
  
  using StrategiesStringVector = std::vector<std::string>;
  
  using indexesVector = std::vector<std::vector<size_t>>;
  
  class GameInfo {
	public:
	  GameInfo(StrategiesStringVector &strategies, const std::string &mode, const std::string &storagePath = "");
	  GameInfo(std::ifstream &file, StrategiesStringVector &strategies, const std::string &mode,
			   const std::string &storagePath = "");
	  void start(int steps = 1);
	  static indexesVector generateCombinationIndexes(size_t n, size_t k);
	private:
	  [[nodiscard]] static StrategiesVector createStrategies(const std::vector<std::string> &strategiesNamesVector);
	  void runSimulation(int n);
	  void makeMove(Strategies::MoveType move);
	  void sumPoints(const std::vector<size_t> &indexes, const PointsTriple &pointsInCurrentStep);
	  void printCurrentTripleState(const PointsTriple &current_points) const;
	  void runTournament();
	  void printStrategyPoints() const;
	  [[nodiscard]] std::array<std::string, 3> fillNamesArray();
	  
	  StrategiesVector strategies;
	  PrisonerConfig::Matrix matrix;
	  std::vector<Strategies::MoveType> moves;
	  std::string mode;
	  std::shared_ptr<FileStorage> storage;
	  
  };
	
}

