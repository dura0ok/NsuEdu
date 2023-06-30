//
// Created by stepan7 on 27.07.22.
//

#include <fstream>
#include <sstream>
#include "ReadFileBlock.h"
#include "../../exceptions/FileNotFoundException.h"


std::string ReadFileBlock::operation(std::string last_result) {
    std::ifstream inFile;
    inFile.open(this->args[0]); //open the input file
    if (inFile.fail()) throw FileNotFoundException(this->args[0]);

    std::stringstream strStream;
    strStream << inFile.rdbuf(); //read the file
    std::string str = strStream.str(); //str holds the content of the file

    return str;
}
