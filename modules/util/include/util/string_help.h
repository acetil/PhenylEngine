#pragma once

#include <algorithm>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>
#include <cstring>
#include <cstdint>
#include <limits>
#include <optional>

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

    std::string_view StringTrim (std::string_view s);

    namespace detail {
        struct StringSplitter {
        private:
            std::string_view m_str;
            std::string_view m_delim;
            std::size_t m_maxSplits;

            struct Sentinel {

            };

            struct Iterator {
            private:
                const StringSplitter* m_splitter = nullptr;
                std::size_t m_pos = 0;
                std::size_t m_numSplits = 0;
                std::optional<std::string_view> m_curr;

                void findNext ();
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
            };
        public:
            using iterator = Iterator;
            using sentinel = Sentinel;

            StringSplitter (std::string_view s, std::string_view delim, std::size_t maxSplits);

            iterator begin () const;
            sentinel end () const;
        };
    }

    detail::StringSplitter StringSplit (std::string_view s, std::string_view delim=" ", std::size_t maxSplits = std::numeric_limits<std::size_t>::max());
}
