#include "string_help.h"

using namespace util;

std::vector<std::string> stringSplit (std::string& str) {
    std::vector<std::string> substrings;

    std::size_t start = 0;
    auto end = std::string::npos;

    while ((end = str.find(' ', start)) != std::string::npos) {
        substrings.emplace_back(str.substr(start, end - start));
        start = end + 1;
    }
    substrings.emplace_back(str.substr(start));
    return substrings;
}

std::vector<std::string> util::stringSplit (const std::string& str, const std::string& delim, int maxSplits) {
    std::vector<std::string> substrings;

    std::size_t start = 0;
    std::size_t end = std::string::npos;
    int i = 0;
    while ((end = str.find(delim, start)) != std::string::npos && i < maxSplits) {
        substrings.emplace_back(str.substr(start, end - start));
        start = end + delim.size();
        i++;
    }
    substrings.emplace_back(str.substr(start));

    return substrings;
}

std::vector<std::string> util::readLines (std::ifstream& file, int maxLines) {
    std::vector<std::string> lines;

    std::string line;

    while (lines.size() < maxLines && std::getline(file, line)) {
        lines.emplace_back(line);
    }

    return lines;
}