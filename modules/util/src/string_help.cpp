#include <cctype>
#include <sstream>

#include "util/string_help.h"

#include "logging/logging.h"

using namespace phenyl;

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

std::string util::joinStrings (const std::string& joinStr, const std::vector<std::string>& strs) {
    std::ostringstream ss;
    int i = 1;
    for (auto& s : strs) {
        ss << s;
        if (i < strs.size()) {
            ss << joinStr;
        }
        i++;
    }
    return std::move(ss.str());
}

std::string_view util::StringTrim (std::string_view s) {
    auto startIt = s.begin();
    auto endIt = s.end();

    while (startIt != endIt && std::isspace(*startIt)) {
        ++startIt;
    }

    while (endIt != startIt && std::isspace(*std::prev(endIt))) {
        --endIt;
    }

    return std::string_view{startIt, endIt};
}

util::detail::StringSplitter util::StringSplit (std::string_view s, std::string_view delim, std::size_t maxSplits) {
    return detail::StringSplitter{s, delim, maxSplits};
}


static_assert(std::ranges::range<util::detail::StringSplitter>);

util::detail::StringSplitter::StringSplitter (std::string_view s, std::string_view delim, std::size_t maxSplits) : s{s}, delim{delim}, maxSplits{maxSplits} {}

util::detail::StringSplitter::iterator util::detail::StringSplitter::begin () const {
    return Iterator{this};
}

util::detail::StringSplitter::sentinel util::detail::StringSplitter::end () const {
    return Sentinel{};
}

util::detail::StringSplitter::Iterator::Iterator (const StringSplitter* splitter) : splitter{splitter} {
    findNext();
}

std::string_view util::detail::StringSplitter::Iterator::operator* () const {
    return *curr;
}

util::detail::StringSplitter::Iterator& util::detail::StringSplitter::Iterator::operator++ () {
    findNext();
    return *this;
}

util::detail::StringSplitter::Iterator util::detail::StringSplitter::Iterator::operator++ (int) {
    auto copy = *this;
    ++*this;

    return copy;
}

bool util::detail::StringSplitter::Iterator::operator== (const Iterator& other) const noexcept {
    return splitter == other.splitter && pos == other.pos;
}

bool util::detail::StringSplitter::Iterator::operator== (const Sentinel& other) const noexcept {
    return pos == std::string_view::npos && !curr;
}

void util::detail::StringSplitter::Iterator::findNext () {
    PHENYL_DASSERT(splitter);
    if (pos == std::string_view::npos || pos == splitter->s.size()) {
        curr = std::nullopt;
        return;
    }

    if (numSplits + 1 == splitter->maxSplits) {
        curr = splitter->s.substr(pos);
        pos = std::string_view::npos;
    } else {
        auto end = splitter->s.find(splitter->delim, pos);

        if (end != std::string_view::npos) {
            curr = splitter->s.substr(pos, end - pos);
            pos = end + splitter->delim.size();
        } else {
            curr = splitter->s.substr(pos);
            pos = std::string_view::npos;
        }
    }

    numSplits++;
}
