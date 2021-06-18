#include <fstream>
#include <string>
#include <vector>

#ifndef STRING_HELP_H
#define STRING_HELP_H
namespace util {
    std::vector<std::string> stringSplit (const std::string& str, const std::string& delim = " ");
    std::vector<std::string> readLines (std::ifstream& file);
}
#endif //STRING_HELP_H
