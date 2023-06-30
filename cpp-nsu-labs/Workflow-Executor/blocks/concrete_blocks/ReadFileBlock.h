#pragma once
#include "../Block.h"
#include <iostream>


class ReadFileBlock : public Block {
public:
    [[nodiscard]] std::string operation(std::string last_result) override;

    ReadFileBlock(unsigned long id, const std::vector <std::string> &args) :Block(id, args){};
};