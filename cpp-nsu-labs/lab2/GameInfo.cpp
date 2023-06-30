#include <iostream>
#include <algorithm>
#include <utility>
#include <vector>
#include "GameInfo.h"
#include "strategies/StrategyFactory.h"
#include "helpers/LineHelper.h"
#include "Matrix.h"
#include "gameHistory/FileStorage.h"

namespace PrisonerConfig {
  
  const inline size_t defaultPlayersCount = 3;
  
  using std::cout;
  using std::endl;
  using std::string;
  
  GameInfo::GameInfo(StrategiesStringVector &list, const string &mode, const string &storagePath)
	  : strategies(createStrategies(list)), mode(std::string(mode)),
		storage(std::make_shared<FileStorage>(storagePath)) {
	  matrix = Matrix();
  }
  
  GameInfo::GameInfo(std::ifstream &file, StrategiesStringVector &list, const string &mode, const string &storagePath)
	  : strategies(createStrategies(list)), mode(std::string(mode)),
		storage(std::make_shared<FileStorage>(storagePath)) {
	  if (file.fail()) {
		  throw std::runtime_error("no default_matrix.txt found in current path");
	  }
	  matrix = Matrix(file);
	  if (!matrix.isMatrixValid()) {
		  throw std::invalid_argument("matrix is invalid");
	  }
  }
  
  StrategiesVector GameInfo::createStrategies(const std::vector<std::string> &strategiesNamesVector) {
	  auto strategyFactory = Strategies::StrategyFactory();
	  StrategiesVector res;
	  for (auto const &strategyName : strategiesNamesVector) {
		  auto newStrategy = strategyFactory.create(strategyName);
		  res.push_back({std::move(newStrategy), 0});
	  }
	  
	  return res;
  }
  
  void GameInfo::makeMove(Strategies::MoveType move) {
	  if (move != Strategies::COLLUDE && move != Strategies::DEFECT) {
		  throw std::runtime_error("invalid move");
	  }
	  moves.push_back(move);
  }
  
  void GameInfo::start(int steps) {
	  if (steps <= 0) {
		  if (steps == 0) {
			  cout << "empty game";
			  return;
		  }
		  throw std::invalid_argument("steps argument must be positive");
	  }
	  
	  if (mode == "detailed") {
		  cout << "Starting game with mode: " + mode << ". Steps: " << steps << endl;
	  }
	  if (mode == "detailed" || mode == "fast") {
		  return runSimulation(steps);
	  }
	  
	  if (mode == "tournament") {
		  return runTournament();
	  }
  }
  
  void GameInfo::runSimulation(int steps) {
	  const std::vector<size_t> indexes = {0, 1, 2};
	  for (int step = 0; step < steps; step++) {
		  if (mode == "detailed") {
			  cout << "Round " << step + 1 << endl;
		  }
		  std::array<Strategies::MoveType, defaultPlayersCount> generatedMovesInTurn{};
		  for (size_t i = 0; i < defaultPlayersCount; i++) {
			  auto rivals = std::make_tuple(
				  strategies[(i + 1) % 3].first->getStrategyName(),
				  strategies[(i + 2) % 3].first->getStrategyName()
			  );
			  
			  auto context = Strategies::Context(moves, rivals, storage);
			  auto generatedMove = strategies[i].first->generateMove(context);
			  generatedMovesInTurn[i] = generatedMove;
			  if (mode == "detailed") {
				  cout << strategies[i].first->getStrategyName() << " generating move: "
					   << static_cast<char>(generatedMove);
				  std::cin.get();
			  }
			  makeMove(generatedMove);
		  }
		  
		  auto pointsInCurrentStep = matrix[generatedMovesInTurn];
		  sumPoints(indexes, pointsInCurrentStep);
		  
		  if (mode == "detailed") {
			  cout << "Points from round: " << endl;
			  printCurrentTripleState(pointsInCurrentStep);
			  cout << "End round " << endl << endl;
		  }
	  }
	  
	  if (mode == "detailed") {
		  for (const auto &move : moves) {
			  cout << static_cast<char>(move);
		  }
		  cout << endl;
	  }
	  
	  if (storage->isStorageOK()) {
		  auto names = fillNamesArray();
		  storage->writeGame(names, moves);
	  }
	  
	  cout << "Points in the end of the game: " << endl;
	  printStrategyPoints();
	  
  }
  Names GameInfo::fillNamesArray() {
	  Names names;
	  for (size_t i = 0; i < defaultPlayersCount; ++i) {
		  names[i] = strategies[i].first->getStrategyName();
	  }
	  return names;
  }
  void GameInfo::printStrategyPoints() const {
	  for (auto &strategy : strategies) {
		  cout << "Strategy: " << strategy.first->getStrategyName() << " " << strategy.second << endl;
	  }
  }
  void GameInfo::sumPoints(const std::vector<size_t> &indexes, const PointsTriple &pointsInCurrentStep) {
	  for (size_t i = 0; i < indexes.size(); ++i) {
		  strategies[indexes[i]].second += pointsInCurrentStep[i];
	  }
  }
  
  void GameInfo::runTournament() {
	  
	  const auto
		  indexes = generateCombinationIndexes(strategies.size(), defaultPlayersCount);
	  for (const auto &setOfIndexes : indexes) {
		  std::array<Strategies::MoveType, defaultPlayersCount> generatedMovesInTurn{};
		  size_t turnMoveIndex = 0;
		  Names names;
		  for (const auto &i : setOfIndexes) {
			  auto rivals = std::make_tuple(
				  strategies[(i + 1) % 3].first->getStrategyName(),
				  strategies[(i + 2) % 3].first->getStrategyName()
			  );
			  
			  auto context = Strategies::Context(moves, rivals, storage);
			  auto generatedMove = strategies[i].first->generateMove(context);
			  names[turnMoveIndex] = strategies[i].first->getStrategyName();
			  generatedMovesInTurn[turnMoveIndex] = generatedMove;
			  makeMove(generatedMove);
			  turnMoveIndex++;
		  }
		  
		  auto pointsInCurrentStep = matrix[generatedMovesInTurn];
		  
		  if (storage->isStorageOK()) {
			  storage->writeGame(names, moves);
		  }
		  
		  moves.clear();
		  sumPoints(setOfIndexes, pointsInCurrentStep);
		  cout << "Tournament round result " << endl;
		  for (const auto &index : setOfIndexes) {
			  cout << strategies[index].first->getStrategyName() << " " << strategies[index].second << endl;
		  }
		  cout << "==================" << endl << endl;
	  }
	  cout << "After all rounds" << endl;
	  printStrategyPoints();
  }
  
  void GameInfo::printCurrentTripleState(const PointsTriple &current_points) const {
	  for (size_t i = 0; i < strategies.size(); i++) {
		  cout << strategies[i].first->getStrategyName() << ": " << current_points[i] << endl;
	  }
  }
  indexesVector GameInfo::generateCombinationIndexes(size_t n, size_t k) {
	  indexesVector res;
	  std::string bitmask(k, 1);
	  bitmask.resize(n, 0);
	  do {
		  std::vector<size_t> tmp;
		  for (size_t i = 0; i < n; ++i) {
			  if (bitmask[i]) {
				  tmp.push_back(i);
			  }
		  }
		  res.push_back(tmp);
	  } while (std::prev_permutation(bitmask.begin(), bitmask.end()));
	  return res;
  }
	
}