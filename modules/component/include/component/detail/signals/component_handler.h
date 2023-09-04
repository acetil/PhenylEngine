#pragma once

#include <functional>
#include <vector>

#include "component/signals/component_update.h"
#include "component/detail/component_utils.h"
#include "component/forward.h"

#include "component_handler_base.h"
#include "component/entity.h"

namespace component::detail {
    template <typename T, TypedUntypedComponentSignal<T> SignalUntyped>
    class TypedComponentSignalHandler : public ComponentSignalHandler<SignalUntyped> {
    private:
        using Signal = SignalUntyped::template Signal<T>;
        std::vector<std::function<void(Entity, const Signal&)>> handlers;
    protected:
        void handleInt (BasicManager* manager, EntityId id, const SignalUntyped& signalUntyped) override {
            Signal signal = signalUntyped.template convert<T>();
            auto entity = manager->_entity(id);

            for (auto& i : handlers) {
                if (!entity.exists()) {
                    break;
                }
                i(entity, signal);
            }
        }
    public:
        void addHandler (std::function<void(Entity, const Signal&)> handler) {
            handlers.emplace_back(std::move(handler));
        }
    };
}