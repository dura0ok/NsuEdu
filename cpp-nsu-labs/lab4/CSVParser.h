#pragma once
#include <iostream>
#include <filesystem>
#include <vector>
#include <string_view>
#include <fstream>
#include "Printer.h"

const inline char NULL_TERMINATOR = '\0';

namespace Details {
	template<typename T>
	T convert(const std::string &s, std::size_t N) {
	  std::stringstream convertStream(s);

	  T value;
	  convertStream >> value;
	  if (convertStream.fail() || !convertStream.eof()) {
		throw std::invalid_argument("Row: " + std::to_string(N + 1) + "\n--- " + s);
	  }


	  return value;
	}

	template<typename ...Types, std::size_t... Indices>
	auto vectorToTupleHelper(const std::vector<std::string> &v, std::index_sequence<Indices...>) {
	  return std::make_tuple(convert<Types>(std::string(v[Indices]), std::size_t(Indices))...);
	}

	template<std::size_t N, typename ...Types>
	auto vectorToTuple(const std::vector<std::string> &v) {
	  return vectorToTupleHelper<Types...>(v, std::make_index_sequence<N>{});
	}

	template<typename>
	struct string_t {
	  using type = std::string;
	};

}

namespace CSV {



	template<typename ...Types>
	class CSVParser {
	 public:
	  using DataTuple = std::tuple<Types...>;
	  using ColumnNames = std::tuple<typename Details::string_t<Types>::type...>;
	  const ColumnNames &getNames();

	  explicit CSVParser(const std::filesystem::path &inputFileName,
						 size_t skipLines = 0,
						 bool keysEnabled = false,
						 char newLineSeparator = '\n',
						 char newColumnSeparator = ',',
						 char escapingSymbol = '"');

	  class Iterator;
	  const Iterator begin();
	  const Iterator end();

	 private:
	  ColumnNames names;
	  std::filesystem::path inputFileName;
	  size_t skipLinesCount;
	  bool keysEnabled = false;
	  char lineSeparator;
	  char columnSeparator;
	  char escapingSymbol;

	};

	template<typename... Types>
	const typename CSVParser<Types...>::ColumnNames& CSVParser<Types...>::getNames() {
	  if (!keysEnabled) {
		throw std::runtime_error("You can`t get names, because you not enabled this feature");
	  }

	  begin();
	  return names;
	}

	template<typename... Types>
	const typename CSVParser<Types...>::Iterator CSVParser<Types...>::begin() {
	  return Iterator(*this);
	}

	template<typename... Types>
	const typename CSVParser<Types...>::Iterator CSVParser<Types...>::end() {
	  return Iterator(*this, true);
	}


	template<typename... Types>
	CSVParser<Types...>::CSVParser(const std::filesystem::path &inputFileName,
								   size_t skipLines,
								   bool keysEnabled,
								   char newLineSeparator,
								   char newColumnSeparator,
								   char newEscapingSymbol): inputFileName(inputFileName),
															skipLinesCount(skipLines),
															keysEnabled(keysEnabled),
															lineSeparator(newLineSeparator),
															columnSeparator(newColumnSeparator),
															escapingSymbol(newEscapingSymbol) {

	}

	template<typename... Types>
	class CSVParser<Types...>::Iterator {
	  using DataTuple = typename CSVParser<Types...>::DataTuple;
	 public:
	  explicit Iterator(CSVParser &parser, bool isEOF = false);
	  const DataTuple &operator*();
	  Iterator &operator++();
	  Iterator &operator+=(size_t n);

	  bool operator==(const Iterator &other);
	  bool operator!=(const Iterator &other);

	 private:
	  DataTuple tuple;
	  template<typename... TypesToParse>
	  typename CSVParser<TypesToParse...>::DataTuple parseLine();
	  std::string processLine();
	  template<typename... TypesToParse>
	  std::vector<std::string> getTokens(const std::string &line);
	  void skipLines();
	  void updateRecord();
	  CSVParser<Types...> &parser;
	  std::ifstream file;
	  size_t currentLineCounter = 1;
	  bool isEOF = false;

	};

	template<typename... Types>
	template<typename... TypesToParse>
	std::vector<std::string> CSVParser<Types...>::Iterator::getTokens(const std::string &line) {
	  std::string token;
	  std::vector<std::string> tokens;

	  std::stringstream tempStream(line);
	  char currentChar;
	  char previousChar = NULL_TERMINATOR;
	  std::string tempString;
	  while (tempStream.get(currentChar)) {
		if (previousChar == parser.escapingSymbol && currentChar == parser.columnSeparator) {
		  tempString.pop_back();
		  tempString.push_back(parser.columnSeparator);
		  continue;
		}

		if (currentChar == parser.columnSeparator) {
		  tokens.push_back(tempString);
		  tempString.clear();
		  continue;
		}

		tempString.push_back(currentChar);
		previousChar = currentChar;
	  }

	  if (!tempString.empty()) {
		tokens.push_back(tempString);
	  }

	  return tokens;
	}

	template<typename... Types>
	std::string CSVParser<Types...>::Iterator::processLine() {
	  char currentChar;
	  char previousChar = '\0';
	  std::string tempString;

	  while (file.get(currentChar)) {
		if (previousChar == parser.escapingSymbol && currentChar == parser.lineSeparator) {
		  tempString.pop_back();
		  tempString.push_back(parser.lineSeparator);
		  continue;
		}

		if (currentChar == parser.lineSeparator) {
		  break;
		}

		tempString.push_back(currentChar);
		previousChar = currentChar;
	  }

	  return tempString;
	}

	template<typename... Types>
	void CSVParser<Types...>::Iterator::skipLines() {
	  for (size_t i = 0; i < parser.skipLinesCount; i++) {
		processLine();
	  }
	}

	template<typename... Types>
	bool CSVParser<Types...>::Iterator::operator!=(const Iterator &other) {
	  return !(*this == other);
	}

	template<typename... Types>
	bool CSVParser<Types...>::Iterator::operator==(const Iterator &other) {
	  return (isEOF && other.isEOF) || (!isEOF && !other.isEOF && (currentLineCounter == other.currentLineCounter));
	}

	template<typename... Types>
	typename CSVParser<Types...>::Iterator &CSVParser<Types...>::Iterator::operator++() {
	  updateRecord();
	  isEOF = file.eof();
	  return *this;
	}

	template<typename... Types>
	typename CSVParser<Types...>::Iterator &CSVParser<Types...>::Iterator::operator+=(size_t n) {
	  for (size_t i = 0; i < n; i++) {
		updateRecord();
		isEOF = file.eof();
		if (isEOF) {
		  break;
		}
	  }
	  return *this;
	}

	template<typename... Types>
	template<typename... TypesToParse>
	typename CSVParser<TypesToParse...>::DataTuple CSVParser<Types...>::Iterator::parseLine() {
	  auto line = processLine();
	  if(line.empty()){
		isEOF = true;
		return {};
	  }

	  auto tokens = getTokens<TypesToParse...>(line);

	  if (tokens.empty()) {
		isEOF = true;
		return {};
	  }

	  if(tokens.size() != sizeof...(TypesToParse)){
		throw std::runtime_error("not enough columns");
	  }

	  try {
		return Details::vectorToTuple<sizeof...(TypesToParse), TypesToParse...>(tokens);
	  } catch (...) {
		std::throw_with_nested(std::runtime_error("Error in line " + std::to_string(currentLineCounter)));
	  }
	}

	template<typename... Types>
	void CSVParser<Types...>::Iterator::updateRecord() {
	  if (isEOF) {
		return;
	  }

	  tuple = parseLine<Types...>();
	  currentLineCounter++;
	}

	template<typename... Types>
	const typename CSVParser<Types...>::DataTuple &CSVParser<Types...>::Iterator::operator*() {
	  return tuple;
	}

	template<typename... Types>
	CSVParser<Types...>::Iterator::Iterator(CSVParser &parser, bool isEOF): parser(parser), isEOF(isEOF) {
	  file.open(parser.inputFileName);

	  if (parser.keysEnabled) {
		parser.names = parseLine<typename Details::string_t<Types>::type...>();
	  }

	  skipLines();
	  if (!file.is_open()) {
		throw std::runtime_error("File open error");
	  }


	  updateRecord();
	}

}

