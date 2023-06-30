#pragma once
#include <array>
#include "strategies/Types.h"

namespace PrisonerConfig {
  
  const inline int matrixElementArraySize = 3;
  using MovesTriple = std::array<Strategies::MoveType, matrixElementArraySize>;
  using PointsTriple = std::array<int, matrixElementArraySize>;
  using TripleMap = std::map<MovesTriple, PointsTriple>;
  
  class Matrix {
	  friend bool operator==(const Matrix &first, const Matrix &second);
	public:
	  Matrix();
	  explicit Matrix(std::istream &file);
	  explicit Matrix(const TripleMap &map);
	  [[nodiscard]] bool isMatrixValid() const;
	  PointsTriple operator[](const MovesTriple &moves) const;
	private:
	  TripleMap matrix;
	  static TripleMap getDefaultMatrix();
  };
	
}