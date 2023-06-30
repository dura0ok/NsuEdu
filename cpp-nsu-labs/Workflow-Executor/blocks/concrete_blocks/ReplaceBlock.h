#pragma once

#include "../Block.h"

class ReplaceBlock : public Block {
public:
    [[nodiscard]] std::string operation(std::string last_result) override;
    ReplaceBlock(unsigned long id, const std::vector <std::string> &args) :Block(id, args){};

};