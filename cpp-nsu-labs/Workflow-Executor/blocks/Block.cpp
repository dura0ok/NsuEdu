
#include <vector>
#include "Block.h"

Block::Block(unsigned long id, const std::vector <std::string> &args) : id(id), args(args) {
    this->id = id;
    this->args = args;
}

Block::Block() = default;
