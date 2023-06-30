#pragma once

#include <fstream>
#include <vector>
#include <memory>
#include "../blocks/Block.h"

class Config {
private:
    std::ifstream file;

    std::pair<bool, size_t> FindLineWithContent(const std::string &search);

    std::vector<std::string> GetRawBlocksInfo(size_t from, size_t to);

public:
    std::string name;

    explicit Config(const std::string &name);

    std::vector<std::unique_ptr<Block>> ParseConfig();

    std::vector<std::string> ParseConveyor();
};

