#pragma once

#include <cctype>
#include <fstream>
#include <string>

inline std::string& safeReplace(std::string& str, const std::string& out, const std::string& in = "") {
    auto pos = str.find(out);
    if (pos != std::string::npos)
        return str.replace(pos,out.size(),in);
    return str;
}

inline void getNumber(std::istream& file, int& n) {
    n = 0;
    char c;
    while (file.get(c) && !std::isdigit(static_cast<unsigned char>(c))) {}
    do {
        n = n*10+(c-'0');
    } while (file.get(c) && std::isdigit(static_cast<unsigned char>(c)));
}