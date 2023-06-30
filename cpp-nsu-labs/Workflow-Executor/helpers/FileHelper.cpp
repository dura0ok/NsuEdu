//
// Created by stepan7 on 09.08.22.
//

#include "FileHelper.h"

void FileHelper::ClearProgress(std::ifstream &file) {
    file.clear();
    file.seekg(0);
}

std::string FileHelper::ReadNthLine(std::ifstream &file, size_t n) {
    std::string s;
    //skip N lines
    for (int i = 0; i < n; ++i)
        std::getline(file, s);

    std::getline(file, s);
    return s;
}