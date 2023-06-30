#pragma once
#include <iostream>
#include <cstring>

class FileNotFoundException : public std::exception {
public:
    std::string errorText;

    explicit FileNotFoundException(const std::string &fileName);

    [[nodiscard]] const char *what() const noexcept override;
};


