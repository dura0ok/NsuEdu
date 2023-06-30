//
// Created by stepan7 on 28.07.22.
//

#include "ReplaceBlock.h"
#include <regex>

std::string ReplaceBlock::operation(std::string last_result) {
    std::string replacedString = last_result;
    replacedString = std::regex_replace(replacedString, std::regex(this->args[0]), this->args[1]);
    return replacedString;
}
