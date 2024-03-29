#pragma once

#include <algorithm>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>
#include <cstring>
#include <cstdint>

namespace phenyl::util {
    std::vector<std::string> stringSplit (const std::string& str, const std::string& delim = " ", int maxSplits = INT32_MAX);
    std::vector<std::string> readLines (std::ifstream& file, int maxLines = INT32_MAX);

    std::string joinStrings (const std::string& joinStr, const std::vector<std::string>& strs);

    // Be very careful about endianness if storing/reading result of this!
    template <typename T>
    std::string binToString (T data) {
        void* ptr = &data;

        return std::string((unsigned char*)ptr, ((unsigned char*)ptr) + sizeof(T));
    };

    template<typename T>
    std::string_view stringToBin (std::string_view str, T& data) {
        if (str.size() < sizeof(T)) {
            return str;
        }
        void* dataPtr = &data;

        memcpy(dataPtr, str.data(), sizeof(T));

        return str.substr(sizeof(T));
    };

    inline std::string ToLower (std::string str) {
        std::ranges::transform(str, str.begin(), [] (const auto c) { return std::tolower(c); });
        return str;
    }

    template <typename T>
    std::string ToLower (T&& str) requires (std::constructible_from<std::string, T>) {
        return ToLower(std::string{str});
    }
}
