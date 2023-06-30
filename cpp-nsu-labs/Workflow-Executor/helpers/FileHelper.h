#pragma once

#include <fstream>

class FileHelper {
public:
    static void ClearProgress(std::ifstream &file);

    static std::string ReadNthLine(std::ifstream &file, size_t n);
};
