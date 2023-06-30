#pragma once
#include <vector>
#include <string>
#include "../blocks/Block.h"
#include "../blocks/Creator.h"
#include <memory>
#include <regex>


class LineHelper {
public:
    static std::unique_ptr<Block> ParseLineToBlock(const std::vector<std::string> &splitLine);

    static std::vector<std::string> SplitStringByTokens(const std::string &s, const std::regex &re);

    static std::vector<std::string> SplitStringBySubMatches(const std::string &s, const std::regex &re);
};
