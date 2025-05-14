#pragma once

#include <concepts>
#include <format>
#include <functional>
#include <type_traits>

namespace phenyl::meta {
class TypeIndex {
public:
    TypeIndex () = default;

    template <typename T>
    static TypeIndex Get () {
        return TypeIndex{Impl<std::remove_cvref_t<T>>::Val()};
    }

    explicit operator bool () const noexcept {
        return m_index;
    }

    bool operator== (const TypeIndex&) const = default;
    auto operator<=> (const TypeIndex&) const = default;

    [[nodiscard]] std::size_t hash () const noexcept {
        return std::hash<std::size_t>{}(m_index);
    }

private:
    struct CurrIndex {
        static std::size_t GetNext () {
            static std::size_t val = 1;
            return val++;
        }
    };

    template <typename T>
    struct Impl {
        static std::size_t Val () {
            static std::size_t val = CurrIndex::GetNext();
            return val;
        }
    };

    std::size_t m_index{};

    explicit TypeIndex (std::size_t index) : m_index{index} {}
    friend struct std::formatter<phenyl::meta::TypeIndex, char>;
};
} // namespace phenyl::meta

template <>
struct std::formatter<phenyl::meta::TypeIndex, char> {
    template <class ParseContext>
    constexpr ParseContext::iterator parse (ParseContext& ctx) {
        auto it = ctx.begin();
        if (it != ctx.end() && *it != '}') {
            throw std::format_error("Invalid format string for TypeIndex");
        }

        return it;
    }

    template <class FmtContext>
    FmtContext::iterator format (const phenyl::meta::TypeIndex& type, FmtContext& ctx) const {
        return std::format_to(ctx.out(), "{}", type.m_index);
    }
};

template <>
struct std::hash<phenyl::meta::TypeIndex> {
    std::size_t operator() (const phenyl::meta::TypeIndex& type) const noexcept {
        return type.hash();
    }
};
