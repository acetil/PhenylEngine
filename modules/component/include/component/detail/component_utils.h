#pragma once

#include <cstddef>
#include <tuple>

#include "component/entity_id.h"

namespace phenyl::component::detail {
    template<std::size_t N, typename ...Args>
    bool tupleAllNonNull (const std::tuple<std::remove_reference_t<Args>* ...>& tup) {
        if constexpr (N == sizeof...(Args)) {
            return true;
        } else {
            return tupleAllNonNull<N + 1, Args...>(tup) && std::get<N>(tup);
        }
    }

    template<typename ...Args>
    bool tupleAllNonNull (const std::tuple<std::remove_reference_t<Args>* ...>& tup) {
        return tupleAllNonNull<0, Args...>(tup);
    }
}