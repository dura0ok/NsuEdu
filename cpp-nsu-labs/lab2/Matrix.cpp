#include <string>
#include <regex>
#include "Matrix.h"
#include "helpers/LineHelper.h"

namespace PrisonerConfig {
  
  using Strategies::MoveType;
  constexpr auto C = Strategies::MoveType::COLLUDE;
  
  constexpr auto D = Strategies::MoveType::DEFECT;
  
  const inline int matrixSize = 8;
  const inline char commentSign = '#';
  
  TripleMap Matrix::getDefaultMatrix() {
	  return {
		  {{C, C, C}, {7, 7, 7}},
		  {{C, C, D}, {3, 3, 9}},
		  {{C, D, C}, {3, 9, 3}},
		  {{D, C, C}, {9, 3, 3}},
		  {{C, D, D}, {0, 5, 5}},
		  {{D, C, D}, {5, 0, 5}},
		  {{D, D, C}, {5, 5, 0}},
		  {{D, D, D}, {1, 1, 1}},
	  };
  }
  
  Matrix::Matrix() {
	  matrix = getDefaultMatrix();
  }
  
  Matrix::Matrix(const TripleMap &map) {
	  matrix = map;
  }
  
  Matrix::Matrix(std::istream &file) {
	  if (file.fail()) {
		  throw std::runtime_error("bad file");
	  }
	  
	  std::string line;
	  std::smatch regexMatch;
	  std::regex regexp(R"(([C,D])\s+([C,D])\s+([C,D])\s+=>\s+([-+]?\d+)\s+([-+]?\d+)+\s+([-+]?\d+))");
	  TripleMap map = {};
	  
	  while (std::getline(file, line)) {
		  if (!std::regex_search(line, regexMatch, regexp)) {
			  if (line.empty()) {
				  continue;
			  }
			  if (Helpers::trim(line).find(commentSign) == 0) {
				  continue;
			  }
			  throw std::invalid_argument("bad line(parsing matrix) : " + line);
		  }
		  MovesTriple keys = {static_cast<MoveType>(regexMatch.str(1)[0]),
							  static_cast<MoveType>(regexMatch.str(2)[0]),
							  static_cast<MoveType>(regexMatch.str(3)[0])};
		  PointsTriple
			  values = {std::stoi(regexMatch.str(4)), std::stoi(regexMatch.str(5)), std::stoi(regexMatch.str(6))};
		  map.insert({keys, values});
	  }
	  
	  matrix = map;
	  
  }
  
  bool rulesCheck(const TripleMap &map) {
	  auto c0 = map.at({C, C, C})[0];
	  auto c1 = map.at({C, C, D})[0];
	  auto c2 = map.at({D, D, C})[2];
	  
	  auto d0 = map.at({C, C, D})[2];
	  auto d1 = map.at({C, D, D})[2];
	  auto d2 = map.at({D, D, D})[0];
	  
	  auto check1 = 3 * c0 > 2 * c1 + d0;
	  auto check2 = d0 > c0;
	  auto check3 = c0 > d1;
	  auto check4 = d1 > c1;
	  auto check5 = c1 > d2;
	  auto check6 = d2 > c2;
	  
	  return check1 && check2 && check3 && check4 && check5 && check6;
  }
  
  bool symmetryCheck(const TripleMap &map) {
	  auto el1 = map.at({C, C, C});
	  auto el2 = map.at({C, C, D});
	  auto el3 = map.at({C, D, C});
	  auto el4 = map.at({D, C, C});
	  auto el5 = map.at({C, D, D});
	  auto el6 = map.at({D, C, D});
	  auto el7 = map.at({D, D, C});
	  auto el8 = map.at({D, D, D});
	  
	  auto check1 = el1[0] == el1[1] && el1[0] == el1[2];
	  auto check2 = el5[0] == el6[1] && el5[0] == el7[2];
	  auto check3 = el2[0] == el2[1] && el2[0] == el3[0] && el3[0] == el3[2] && el3[2] == el4[1] && el4[1] == el4[2];
	  auto check4 = el2[2] == el3[1] && el2[2] == el4[0];
	  auto check5 = el5[1] == el5[2] && el5[1] == el6[0] && el6[0] == el6[2] && el6[0] == el7[0] && el7[0] == el7[1];
	  auto check6 = el8[0] == el8[1] && el8[0] == el8[2];
	  return check1 && check2 && check3 && check4 && check5 && check6;
  }
  
  bool Matrix::isMatrixValid() const {
	  if (matrix.size() != matrixSize) {
		  return false;
	  }
	  
	  if (!rulesCheck(matrix) || !symmetryCheck(matrix)) {
		  return false;
	  }
	  
	  return true;
  }
  PointsTriple Matrix::operator[](const MovesTriple &moves) const {
	  return matrix.at(moves);
  }
  
  bool operator==(const Matrix &first, const Matrix &second) {
	  return first.matrix == second.matrix;
  }
	
}