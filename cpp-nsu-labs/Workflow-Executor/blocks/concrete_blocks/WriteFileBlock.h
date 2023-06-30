#pragma once

#include "../Block.h"

class WriteFileBlock : public Block {
public:
    [[nodiscard]] std::string operation(std::string last_result) override;

    WriteFileBlock(unsigned long id, const std::vector<std::string> &args) : Block(id, args) {};

};