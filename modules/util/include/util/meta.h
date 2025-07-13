#pragma once

#include <functional>

namespace phenyl::meta {
namespace detail {
    template <typename T>
    struct IsStdFunction {
        static constexpr bool Value = false;
    };

    template <typename R, typename... Args>
    struct IsStdFunction<std::function<R(Args...)>> {
        static constexpr bool Value = true;
    };
} // namespace detail

template <typename T>
static constexpr bool IsStdFunction = detail::IsStdFunction<T>::Value;
} // namespace phenyl::meta
