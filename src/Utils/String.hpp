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

inline bool getNumber(std::istream& file, int& n) {
    n = 0;
    char c;
    bool negative = false;

    while (file.get(c)) {
        if (std::isdigit(static_cast<unsigned char>(c)))
            break;
        if (c == '-' && std::isdigit(file.peek())) {
            negative = true;
            file.get(c);
            break;
        }
    }

    if (!file)
        return false;

    n = (c-'0');
    while (std::isdigit(file.peek())) {
        file.get(c);
        n = n*10+(c-'0');
    }
    
    if (negative)
        n = -n;
    return true;
}