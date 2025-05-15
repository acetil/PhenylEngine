#pragma once

#include <concepts>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace phenyl::util {
template <typename T>
struct Hasher {
    using HashFunc = std::hash<T>;
    using KeyEq = std::equal_to<T>;
};

template <typename T, typename... Args>
struct TransparentHashFunc : public std::hash<T>, public std::hash<Args>... {
    using is_transparent = void;
};

template <typename T, std::equality_comparable_with<T> U>
struct HeterogenousEq {
    constexpr bool operator() (const U& lhs, const T& rhs) const noexcept {
        return lhs == rhs;
    }
};

template <typename T, std::equality_comparable_with<T>... Args>
struct TransparentKeyEq : public std::equal_to<T>, public HeterogenousEq<T, Args>... {
    using is_transparent = void;
};

template <>
struct Hasher<std::string> {
    struct HashFunc {
        using is_transparent = void;

        bool operator() (std::string_view str) const noexcept {
            return std::hash<std::string_view>{}(str);
        }
    };

    struct KeyEq {
        using is_transparent = void;

        bool operator() (const auto& lhs, const std::string& rhs) const noexcept {
            return lhs == rhs;
        }
    };
};

constexpr std::size_t CombineHashes (std::size_t lhs, std::size_t rhs) noexcept {
    // https://stackoverflow.com/questions/5889238/why-is-xor-the-default-way-to-combine-hashes
    return lhs ^ (rhs + 0x517cc1b727220a95 + (lhs << 6) + (lhs >> 2));
}

constexpr std::size_t HashAllInternal (std::size_t seed) noexcept {
    return seed;
}

template <typename T, typename... Args>
constexpr std::size_t HashAllInternal (std::size_t seed, const T& obj, const Args&... args) noexcept {
    auto newVal = CombineHashes(seed, (typename Hasher<T>::HashFunc) {}(obj));
    return HashAllInternal(newVal, args...);
}

template <typename T, typename... Args>
constexpr std::size_t HashAll (const T& obj, const Args&... args) noexcept {
    return HashAllInternal((typename Hasher<T>::HashFunc) {}(obj), args...);
}

template <typename Key, typename Value>
using HashMap = std::unordered_map<Key, Value, typename Hasher<Key>::HashFunc, typename Hasher<Key>::KeyEq>;
} // namespace phenyl::util

template <typename T, typename U>
struct std::hash<std::pair<T, U>> {
    std::size_t operator() (const std::pair<T, U>& p) const noexcept {
        return phenyl::util::HashAll(p.first, p.second);
    }
};

template <typename T>
struct std::hash<std::vector<T>> {
    std::size_t operator() (const std::vector<T>& v) const noexcept {
        std::size_t seed = 0;
        for (auto& i : v) {
            seed = phenyl::util::CombineHashes(seed, (typename phenyl::util::Hasher<T>::HashFunc) {}(i));
        }

        return seed;
    }
};
