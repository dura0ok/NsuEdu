#pragma once

#include "../Creator.h"

class WriteFileBlockCreator : public Creator {
public:
    WriteFileBlockCreator() : Creator() {};

    [[nodiscard]] std::unique_ptr<Block> FactoryMethod(unsigned long id, const std::vector<std::string> &args) override;
};


