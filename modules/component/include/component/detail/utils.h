#pragma once

#include <cstddef>
#include <tuple>

#include "component/entity_id.h"

namespace component {
    namespace detail {
        template <typename Signal>
        class SignalHandlerList;
    }
    class IterInfo {
    private:
        ComponentManager* compManager;
        EntityId eId;

        IterInfo (const IterInfo&) = default;
        IterInfo (IterInfo&&) = default;

        IterInfo& operator= (const IterInfo&) = default;
        IterInfo& operator= (IterInfo&&) = default;

        friend component::ComponentManager;
        template <typename T>
        friend class detail::SignalHandlerList;
        IterInfo (ComponentManager* manager, EntityId id) : compManager{manager}, eId{id} {}
    public:
        [[nodiscard]] constexpr EntityId id () const {
            return eId;
        }

        ComponentManager& manager () {
            return *compManager;
        }

        [[nodiscard]] const ComponentManager& manager () const {
            return *compManager;
        }

        ~IterInfo() = default;
    };
}

namespace component::detail {
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