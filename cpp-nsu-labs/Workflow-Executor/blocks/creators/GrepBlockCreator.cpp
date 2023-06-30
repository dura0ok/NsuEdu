//
// Created by stepan7 on 11.08.22.
//

#include "GrepBlockCreator.h"
#include "../concrete_blocks/GrepBlock.h"

std::unique_ptr<Block> GrepBlockCreator::FactoryMethod(unsigned long id, const std::vector<std::string> &args) {
    return std::make_unique<GrepBlock>(id, args);
}
