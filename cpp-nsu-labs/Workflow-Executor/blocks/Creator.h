#pragma once
#include <iostream>
#include "Block.h"
#include <memory>

class Creator {
public:
    Creator()= default;
    virtual ~Creator()= default;
    [[nodiscard]] virtual std::unique_ptr<Block> FactoryMethod(unsigned long id, const std::vector <std::string> &args) = 0;

    static std::unique_ptr<Creator> mapper(const std::string& command_name);
};