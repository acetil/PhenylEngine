#pragma once

#include "component/signals/component_update.h"
#include "component/forward.h"
#include "component/detail/component_utils.h"


namespace component::detail {
    template <UntypedComponentSignal Signal>
    class ComponentSignalHandler {
    private:
        ComponentSignalHandler<Signal>* parent;
    protected:
        virtual void handleInt (BasicManager* manager, EntityId id, const Signal& signal) = 0;
    public:
        virtual ~ComponentSignalHandler () = default;
        void handle (BasicManager* manager, EntityId id, const Signal& signal) {
            if constexpr (Signal::PreOrder) {
                if (parent) {
                    parent->handle(manager, id, signal);
                }
            }

            handleInt(manager, id, signal);

            if constexpr (!Signal::PreOrder) {
                if (parent) {
                    parent->handle(manager, id, signal);
                }
            }
        }

        void setParent (ComponentSignalHandler<Signal>* newParent) {
            parent = newParent;
        }
    };
}