#include <fstream>
#include "../GameInfo.h"
#include "gtest/gtest.h"

namespace {
  using namespace PrisonerConfig;
  using namespace Strategies;
  using namespace Helpers;
  
  TEST(MatrixParseTest, invalidMatrixSizeTest) {
	  TripleMap expectMatrix = {
		  {{COLLUDE, COLLUDE, COLLUDE}, {7, 7, 7}},
		  {{COLLUDE, COLLUDE, DEFECT}, {3, 3, 9}},
	  };
	  
	  auto res = Matrix(expectMatrix);
	  EXPECT_EQ(Matrix(expectMatrix), res);
	  EXPECT_FALSE(res.isMatrixValid());
  }
  
  TEST(MatrixParseTest, InvalidNumberInRow) {
	  std::ifstream file("../tests/matrix_templates/err1.txt");
	  Matrix matrix;
	  EXPECT_THROW(matrix = Matrix(file), std::invalid_argument);
  }
  
  TEST(MatrixParseTest, NotConsistent) {
	  std::ifstream file("../tests/matrix_templates/not_consistent.txt");
	  Matrix matrix;
	  matrix = Matrix(file);
	  EXPECT_FALSE(matrix.isMatrixValid());
  }
  
  TEST(MatrixParseTest, defaultMatrixTest) {
	  std::ifstream file("../tests/matrix_templates/def.txt");
	  auto res = Matrix(file);
	  EXPECT_EQ(Matrix(), res);
	  EXPECT_TRUE(res.isMatrixValid());
  }
  
  TEST(MatrixParseTest, okNegative) {
	  std::ifstream file("../tests/matrix_templates/correct_negative.txt");
	  auto res = Matrix(file);
	  EXPECT_TRUE(res.isMatrixValid());
  }
  
  TEST(MatrixParseTest, okCommentSpaces) {
	  std::ifstream file("../tests/matrix_templates/ok_comments_spaces.txt");
	  auto res = Matrix(file);
	  EXPECT_EQ(Matrix(), res);
	  EXPECT_TRUE(res.isMatrixValid());
  };
  
  TEST(MatrixParseTest, AntiSymmetryInvalidTest) {
	  std::ifstream file("../tests/matrix_templates/antisymmetry.txt");
	  auto res = Matrix(file);
	  EXPECT_FALSE(res.isMatrixValid());
  }
  
  TEST(MatrixParseTest, AntiSymmetryInvalidTestNegativeSecond) {
	  std::ifstream file("../tests/matrix_templates/antisymmetry2.txt");
	  auto res = Matrix(file);
	  EXPECT_FALSE(res.isMatrixValid());
  }
  
  TEST(MatrixParseTest, RuleError) {
	  std::ifstream file("../tests/matrix_templates/rule_error.txt");
	  auto res = Matrix(file);
	  EXPECT_FALSE(res.isMatrixValid());
  }
	
}