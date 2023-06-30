//
// Created by stepan7 on 28.07.22.
//

#include "ReplaceBlockCreator.h"
#include "../concrete_blocks/ReplaceBlock.h"

std::unique_ptr<Block> ReplaceBlockCreator::FactoryMethod(unsigned long id, const std::vector<std::string> &args) {
    return std::make_unique<ReplaceBlock>(id, args);
}
