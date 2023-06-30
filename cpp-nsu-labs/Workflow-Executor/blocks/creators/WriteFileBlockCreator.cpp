#include "ReplaceBlockCreator.h"
#include "../concrete_blocks/WriteFileBlock.h"
#include "WriteFileBlockCreator.h"


std::unique_ptr<Block> WriteFileBlockCreator::FactoryMethod(unsigned long id, const std::vector<std::string> &args) {
    return std::make_unique<WriteFileBlock>(id, args);
}
