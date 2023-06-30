#include <sstream>
#include <iostream>
#include "GrepBlock.h"

std::string GrepBlock::operation(std::string last_result) {
    std::vector<std::string> grepLines;
    std::istringstream stringStream(last_result);
    std::string result;
    for (std::string line; std::getline(stringStream, line);) {
        if (line.find(this->args[0]) != std::string::npos) {
            grepLines.push_back(line);
        }
    }

    for (auto &line: grepLines) {
        line.append("\n");
        result += line;
    }
    return result;


}
