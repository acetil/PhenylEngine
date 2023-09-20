#pragma once

#include <array>
#include <tuple>

#include "component/detail/component_set.h"
#include "component/detail/component_utils.h"
#include "handler_list.h"
#include "component/detail/managers/basic_manager.h"
#include "component/entity.h"

#include "util/meta.h"

namespace phenyl::component::detail {
    template <typename Signal, typename F, typename ...Args> requires meta::callable<F, void, const Signal&, Entity, Args&...>
    class TypedSignalHandler : public SignalHandler<Signal> {
    private:
        F fn;
        std::array<ComponentSet*, sizeof...(Args)> components;

        template <std::size_t ...Indexes>
        std::tuple<Args*...> getComponents (EntityId id, std::index_sequence<Indexes...>) {
            return std::make_tuple<Args*...>((Args*) components[Indexes]->getComponentUntyped(id)...);
        }
    public:
        TypedSignalHandler (F fn, std::array<ComponentSet*, sizeof...(Args)> components) : fn{fn}, components{components} {}

        void handle (const Signal& signal, BasicManager* manager, EntityId id) override {
            if (!manager->_exists(id)) {
                return;
            }
            auto comps = getComponents(id, std::make_index_sequence<sizeof...(Args)>{});

            if (tupleAllNonNull<Args...>(comps)) {
                fn(signal, manager->_entity(id), *(std::get<Args*>(comps))...);
            }
        }
    };

}