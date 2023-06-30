#include <string>

#include "gtest/gtest.h"
#include "../CSVParser.h"

namespace {
	using namespace CSV;

	TEST(CSVParserTest, TestEmptyFile) {
	  CSVParser<int, std::string> parser("../tests/csv_files/empty.csv");
	  EXPECT_NO_THROW(parser.begin());
	  auto it = parser.begin();
	  EXPECT_TRUE(it == parser.end());
	  ++it;
	  EXPECT_TRUE(it == parser.end());
	}

	TEST(CSVParserTest, SingleCorrentLine) {
	  CSVParser<int, std::string> parser("../tests/csv_files/single_correct_line.csv");
	  auto line = parser.begin();
	  auto res = std::tuple<int, std::string>(1, "test");
	  EXPECT_TRUE(*line == res);
	}

	TEST(CSVParserTest, MultipleCorrentLines) {
	  CSVParser<int, std::string> parser("../tests/csv_files/multiple_correct_line.csv");
	  auto line = parser.begin();
	  auto firstTuple = std::tuple<int, std::string>(1, "t");
	  EXPECT_TRUE(*line == firstTuple);

	  ++line;
	  auto secondTuple = std::tuple<int, std::string>(2, "t");
	  EXPECT_TRUE(*line == secondTuple);
	}

	TEST(CSVParserTest, SkipLinesTest) {
	  CSVParser<int, std::string> parser("../tests/csv_files/skip_lines.csv", 3);
	  auto line = parser.begin();
	  auto firstTuple = std::tuple<int, std::string>(4, "test4");
	  EXPECT_TRUE(*line == firstTuple);
	}

	TEST(CSVParserTest, SingleBadLine) {
	  CSVParser<int, std::string> parser("../tests/csv_files/bad_single_line.csv");
	  EXPECT_THROW(parser.begin(), std::runtime_error);
	}

	TEST(CSVParserTest, MultipleBadLine) {
	  CSVParser<int, std::string> parser("../tests/csv_files/multiple_bad_line.csv");
	  auto it = parser.begin();
	  EXPECT_THROW(++it;, std::runtime_error);
	}

	TEST(CSVParserTest, AdditionAssignmentOperatorTest) {
	  CSVParser<std::string, int> parser("../tests/csv_files/addition_assignment.csv");
	  auto it = parser.begin();
	  it += 2;
	  auto expectTuple = std::tuple<std::string, int>("t3", 2);
	  EXPECT_TRUE(*it == expectTuple);
	}

	TEST(CSVParserTest, GetColumnNames) {
	  CSVParser<std::string, int> parser("../tests/csv_files/get_column_names.csv", 0, true);
	  EXPECT_EQ(std::get<0>(parser.getNames()), "column1");
	  auto it = parser.begin();
	  auto expectTuple = std::tuple<std::string, int>("test", 1);
	  EXPECT_TRUE(*it == expectTuple);
	}

	TEST(CSVParserTest, BadGetColumnNames) {
	  CSVParser<std::string, int> parser("../tests/csv_files/bad_get_column_names.csv", 0, true);
	  EXPECT_THROW(std::get<0>(parser.getNames()), std::runtime_error);
	}

	TEST(CSVParserTest, EmptyBetweenSeparator){
	  CSVParser<std::string, int> parser("../tests/csv_files/empty_between.csv");
	  EXPECT_THROW(parser.begin(), std::runtime_error);
	}

	TEST(CSVParserTest, EqualityIterators){
	  CSVParser<int, std::string> parser("../tests/csv_files/eq.csv");
	  auto it1 = parser.begin();
	  auto it2 = parser.begin();
	  EXPECT_TRUE(it1 == it2);
	  it2 += 1;
	  EXPECT_FALSE(it1 == it2);
	  it2 += 2;
	  it1 += 3;
	  EXPECT_TRUE(it1 == it2);
	}

	TEST(CSVParserTest, BadLineTest){
	  CSVParser<int, std::string> parser("../tests/csv_files/bad_line.csv");
	  EXPECT_NO_THROW(parser.begin());
	  auto it = parser.begin();
	  EXPECT_THROW(++it, std::runtime_error);
	}

}