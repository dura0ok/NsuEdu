#pragma once

#include <cstdlib>
#include <string>
#include <vector>

class Block {
public:

    Block(size_t id, const std::vector<std::string> &args);

    Block();

    size_t id{};
    std::vector<std::string> args;


    virtual ~Block() = default;

    [[nodiscard]] virtual std::string operation(std::string last_result) = 0;
};

