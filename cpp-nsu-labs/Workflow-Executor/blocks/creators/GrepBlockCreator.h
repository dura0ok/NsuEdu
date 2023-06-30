#pragma once

#include "../Creator.h"

class GrepBlockCreator : public Creator {
public:
    GrepBlockCreator() : Creator() {};

    [[nodiscard]] std::unique_ptr<Block> FactoryMethod(unsigned long id, const std::vector<std::string> &args) override;
};


