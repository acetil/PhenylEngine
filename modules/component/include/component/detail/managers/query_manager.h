#pragma once

#include "basic_manager.h"
#include "component/entity.h"
#include "component/query.h"

namespace phenyl::component::detail {
    class QueryableManager : public BasicManager {
    protected:
        template <typename T>
        [[nodiscard]] bool hasAllComponents () const {
            return getComponent<T>();
        }

        template <typename T, typename ...Args>
        [[nodiscard]] bool hasAllComponents () const requires (sizeof...(Args) != 0) {
            return hasAllComponents<T>() && hasAllComponents<Args...>();
        }

        explicit QueryableManager (std::size_t startCapacity) : BasicManager(startCapacity) {}
    public:
        template <typename ...Args>
        Query<Args...> _query () {
            if (!hasAllComponents<Args...>()) {
                PHENYL_LOGE(MANAGER_LOGGER, "Failed to build query!");
                return Query<Args...>{};
            }

            return Query<Args...>{this, getComponent<Args>()...};
        }

        template <typename ...Args>
        ConstQuery<Args...> _query () const {
            if (!hasAllComponents<Args...>()) {
                PHENYL_LOGE(MANAGER_LOGGER, "Failed to build query!");
                return ConstQuery<Args...>{};
            }

            return ConstQuery<Args...>{this, getComponent<Args>()...};
        }
    };
}