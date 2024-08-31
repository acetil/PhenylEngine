#pragma once

#include <concepts>
#include <memory>
#include <string>

namespace phenyl::util {
    template <typename T>
    struct Hasher {
        using HashFunc = std::hash<T>;
        using KeyEq = std::equal_to<T>;
    };

    template <typename T, typename ...Args>
    struct TransparentHashFunc : public std::hash<T>, public std::hash<Args>... {
        using is_transparent = void;
    };

    template <typename T, std::equality_comparable_with<T> U>
    struct HeterogenousEq {
        constexpr bool operator() (const U& lhs, const T& rhs) const noexcept {
            return lhs == rhs;
        }
    };

    template <typename T, std::equality_comparable_with<T> ...Args>
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

}

template <typename T, typename U>
struct std::hash<std::pair<T, U>> {
    std::size_t operator() (const std::pair<T, U>& p) const noexcept {
        return std::hash<T>{}(p.first) ^ (std::hash<U>{}(p.second) << 1);
    }
};