//
// Created by stepan7 on 12.08.22.
//

#include <fstream>
#include "WriteFileBlock.h"

std::string WriteFileBlock::operation(std::string last_result) {
    std::ofstream out(this->args[0]);
    out << last_result;
    out.close();
    return "";
}
