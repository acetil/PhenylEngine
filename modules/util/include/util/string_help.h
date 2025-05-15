#pragma once

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <limits>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace phenyl::util {
template <std::forward_iterator It, std::sentinel_for<It> S>
std::string JoinStrings (std::string_view joinStr, It begin, S end) {
    if (begin == end) {
        return "";
    }

    auto it = begin;
    std::stringstream str;
    str << *(it++);
    for (; it != end; ++it) {
        str << joinStr << *it;
    }
    return std::move(str).str();
}

template <std::ranges::range R>
std::string JoinStrings (std::string_view joinStr, const R& range) {
    return JoinStrings(joinStr, range.begin(), range.end());
}

inline std::string ToLower (std::string str) {
    std::ranges::transform(str, str.begin(), [] (const auto c) { return std::tolower(c); });
    return str;
}

template <typename T>
std::string ToLower (T&& str) requires (std::constructible_from<std::string, T>)
{
    return ToLower(std::string{str});
}

std::string_view StringTrim (std::string_view s);

namespace detail {
    struct StringSplitter {
    public:
        struct Sentinel {};

        struct Iterator {
        public:
            using value_type = std::string_view;
            using difference_type = std::ptrdiff_t;

            Iterator () = default;
            Iterator (const StringSplitter* splitter);

            std::string_view operator* () const;
            Iterator& operator++ ();
            Iterator operator++ (int);

            bool operator== (const Iterator& other) const noexcept;
            bool operator== (const Sentinel& other) const noexcept;

        private:
            const StringSplitter* m_splitter = nullptr;
            std::size_t m_pos = 0;
            std::size_t m_numSplits = 0;
            std::optional<std::string_view> m_curr;

            void findNext ();
        };

        using iterator = Iterator;
        using sentinel = Sentinel;

        StringSplitter (std::string_view s, std::string_view delim, std::size_t maxSplits);

        iterator begin () const;
        sentinel end () const;

    private:
        std::string_view m_str;
        std::string_view m_delim;
        std::size_t m_maxSplits;
    };
} // namespace detail

detail::StringSplitter StringSplit (std::string_view s, std::string_view delim = " ",
    std::size_t maxSplits = std::numeric_limits<std::size_t>::max());
} // namespace phenyl::util
