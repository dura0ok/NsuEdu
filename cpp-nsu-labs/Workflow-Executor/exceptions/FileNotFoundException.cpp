#include "FileNotFoundException.h"

FileNotFoundException::FileNotFoundException(const std::string &fileName) {
    this->errorText = std::string("File not found: ") + fileName;
}

const char *FileNotFoundException::what() const noexcept {
    return this->errorText.c_str();
}
