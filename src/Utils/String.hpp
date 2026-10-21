#pragma once

#include <cctype>
#include <fstream>
#include <string>

void getNumber(std::istream& file, int& n) {
    n = 0;
    char c;
    while (file.get(c) && !std::isdigit(static_cast<unsigned char>(c))) {}
    do {
        n = n*10+(c-'0');
    } while (file.get(c) && std::isdigit(static_cast<unsigned char>(c)));
}