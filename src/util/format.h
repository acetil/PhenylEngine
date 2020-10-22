#include <string>
#include <sstream>
#include <string_view>
#include <array>
#include <type_traits>
#include <charconv>
#include <iostream>
#ifndef FORMAT_H
#define FORMAT_H
namespace util {
    inline std::string format (std::string formatStr) {
        return formatStr;
    }
    template <int len, int N, std::enable_if_t<N==len, int> = 0>
    std::array<std::string, len> getStringsHelp (std::stringstream& sstream) {
        return std::array<std::string, len>();
    }
    template <int len, int N = 0, typename ...Args>
    std::array<std::string, len> getStringsHelp (std::stringstream& sstream, const std::string& s, Args... args) {
        auto arr = getStringsHelp<len, N + 1>(sstream, args...);
        arr[N] = s;
        //std::cout << arr[N] << std::endl;
        return arr;
    }
    template <int len, int N = 0, typename T, typename ...Args>
    std::array<std::string, len> getStringsHelp (std::stringstream& sstream, T t, Args... args) {
        auto arr = getStringsHelp<len, N + 1>(sstream, args...);
        //std::cout << "Test: " << t << std::endl;
        sstream << t;
        //std::cout << sstream.str() << std::endl;
        arr[N] = sstream.str();
        sstream.str("");
        //std::cout << arr[N] << std::endl;
        return arr;
    }
    template <typename ...Args>
    std::array<std::string, sizeof...(Args)> getStrings (Args... args) {
        std::stringstream sstream;
        return getStringsHelp<sizeof...(args)>(sstream, args...);
    }

    int getFormatStr (std::string_view view, int tot, int currPos);

    template <typename ...Args>
    std::string format (const std::string& formatStr, Args... args) {
        auto arr = getStrings(args...);
        std::string_view view(formatStr);
        std::stringstream outStream;
        std::string outString;
        auto curr = view.begin();
        auto currBegin = curr;
        int formatPos = -1;
        while (curr < view.end()) {
            if (*curr == '{') {
                if (curr + 1 == view.end()) {
                    return "FORMAT ERROR: MISSING CLOSING BRACKET";
                } else {
                    if (*(curr + 1) == '{') {
                        outStream << std::string_view(currBegin, curr + 1);
                        curr += 2;
                        currBegin = curr;
                    } else {
                        auto modView = std::string_view(curr + 1, view.end());
                        if (modView.find_first_of('{') < modView.find_first_of('}')) {
                            return "FORMAT ERROR: OPENING BRACKET BEFORE CLOSING BRACKET";
                        } else {
                            outStream << std::string_view(currBegin, curr);
                            auto currNext = curr + modView.find_first_of('}') + 2;
                            auto newFormatPos = getFormatStr(std::string_view(curr + 1, currNext - 1), sizeof...(args), formatPos);
                            if (newFormatPos == -1) {
                                return "FORMAT ERROR: INCORRECT FORMAT POSITION";
                            } else {
                                outStream << arr[newFormatPos];
                            }
                            formatPos = newFormatPos;
                            curr = currNext;
                            currBegin = curr;
                        }
                    }
                }
            } else if (*curr == '}') {
                if (curr + 1 == view.end()) {
                    return "FORMAT ERROR: UNEXPECTED CLOSING BRACKET";
                }
                if (*(curr + 1) == '}') {
                    outStream << std::string_view (currBegin, curr);
                    curr += 2;
                    currBegin = curr;
                } else {
                    return "FORMAT ERROR: UNEXPECTED CLOSING BRACKET";
                }
            } else {
                curr++;
            }
        }
        if (currBegin < view.end()) {
            outStream << std::string_view(currBegin, view.end());
        }
        outString = std::move(outStream.str());
        return outString;
    }
}

#endif
