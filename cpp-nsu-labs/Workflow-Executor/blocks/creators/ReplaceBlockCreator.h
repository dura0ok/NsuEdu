#pragma once

#include "../Creator.h"

class ReplaceBlockCreator : public Creator{
public:
    ReplaceBlockCreator() :Creator(){};
    [[nodiscard]] std::unique_ptr<Block> FactoryMethod(unsigned long id, const std::vector <std::string> &args) override;
};


