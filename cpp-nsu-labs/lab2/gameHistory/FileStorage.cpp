#include <filesystem>
#include <fstream>
#include <regex>
#include "FileStorage.h"

const inline size_t defaultPlayersCount = 3;

namespace PrisonerConfig {
  
  FileStorage::FileStorage(const std::string &storagePath) {
	  dataFile = std::make_shared<std::fstream>(
		  storagePath, std::fstream::in | std::fstream::out | std::fstream::app);
  }
  
  void FileStorage::writeGame(const Names &strategyNames, const std::vector<Strategies::MoveType> &moves) {
	  if (dataFile->fail()) {
		  throw std::runtime_error("Storage file can`t opened.");
	  }
	  std::string newLine;
	  newLine = strategyNames[0] + "," + strategyNames[1] + "," + strategyNames[2] + ",";
	  for (auto move : moves) {
		  newLine += std::string(1, static_cast<char>(move));
	  }
	  *dataFile << newLine << "\n";
	  dataFile->clear();
  }
  
  StorageElementData FileStorage::loadData(const std::string &firstRival, const std::string &secondRival) {
	  if (dataFile->fail()) {
		  throw std::runtime_error("Storage file can`t opened.");
	  }
	  std::string line;
	  std::smatch regexMatch;
	  std::regex regexp(R"((\w+),(\w+),(\w+),([C,D]+))");
	  StorageElementData res;
	  
	  while (std::getline(*dataFile, line)) {
		  if (!std::regex_search(line, regexMatch, regexp)) {
			  continue;
		  }
		  Names parsedNames;
		  for (size_t i = 1; i < defaultPlayersCount + 1; i++) {
			  parsedNames[i - 1] = regexMatch.str(i);
		  }
		  if (std::find(std::begin(parsedNames), std::end(parsedNames), firstRival) == std::end(parsedNames) &&
			  std::find(std::begin(parsedNames), std::end(parsedNames), secondRival) == std::end(parsedNames)
			  ) {
			  continue;
		  }
		  
		  std::vector<Strategies::MoveType> parsedMoves;
		  for (char &c : regexMatch.str(defaultPlayersCount + 1)) {
			  parsedMoves.push_back(static_cast<Strategies::MoveType>(c));
		  }
		  res.push_back(parsedMoves);
	  }
	  
	  dataFile->clear();
	  return res;
  }
  
  bool FileStorage::isStorageOK() const {
	  return !dataFile->fail();
  }
	
}