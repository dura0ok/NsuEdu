#include <sstream>
#include <fstream>
#include "gtest/gtest.h"
#include "../ConfigParser.h"

namespace {
TEST(ConfigParserTest, EmptyConfigTest) {
  std::ifstream file("../tests/configs/empty.txt");
  auto config = ConfigParser(file);
  auto parsedLines = config.parseConfig({});
  EXPECT_TRUE(parsedLines.empty());
}

TEST(ConfigParserTest, CommandWithoutArgs) {
  auto config = ConfigParser("../tests/configs/command_without_args.txt");
  auto parsedLines = config.parseConfig({});
  ParsedLines res = {{"command"}};
  EXPECT_EQ(res, parsedLines);
}

TEST(ConfigParserTest, CommandSingleArg){
  auto config = ConfigParser("../tests/configs/command_single_arg.txt");
  auto parsedLines = config.parseConfig({});
  ParsedLines res = {{"command", "arg1"}};
  EXPECT_EQ(res, parsedLines);
}

TEST(ConfigParserTest, CommandMultipleArgs){
  auto config = ConfigParser("../tests/configs/command_multiple_args.txt");
  auto parsedLines = config.parseConfig({});
  ParsedLines res = {{"command", "arg1", "arg2", "arg3"}};
  EXPECT_EQ(res, parsedLines);
}

TEST(ConfigParserTest, MultipleCommandsWithArgs){
  auto config = ConfigParser("../tests/configs/multiple_command_with_args.txt");
  auto parsedLines = config.parseConfig({});
  ParsedLines res = {{"command1", "arg1", "arg2"}, {"command2", "arg1", "arg2"}};
  EXPECT_EQ(res, parsedLines);
}

TEST(ConfigParserTest, MultipleCommandsWithArgsCommentsAndEmptyLines){
  auto config = ConfigParser("../tests/configs/multiple_command_with_args_comments_and_spaces.txt");
  auto parsedLines = config.parseConfig({});
  ParsedLines res = {{"command1", "arg1", "arg2"}, {"command2", "arg1", "arg2"}};
  EXPECT_EQ(res, parsedLines);
}



}