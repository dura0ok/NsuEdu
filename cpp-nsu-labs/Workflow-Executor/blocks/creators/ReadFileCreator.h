#pragma once

#include "../Creator.h"
#include "../concrete_blocks/ReadFileBlock.h"

class ReadFileCreator : public Creator{
public:
    ReadFileCreator() :Creator(){};
    [[nodiscard]] std::unique_ptr<Block> FactoryMethod(unsigned long id, const std::vector <std::string> &args) override;
};


