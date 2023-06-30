#pragma once

#include "../Block.h"

class GrepBlock : public Block {
public:
    [[nodiscard]] std::string operation(std::string last_result) override;

    GrepBlock(unsigned long id, const std::vector<std::string> &args) : Block(id, args) {};

};