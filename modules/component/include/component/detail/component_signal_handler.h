#pragma once

#include <functional>
#include <vector>

#include "component/signals/component_update.h"
#include "component_utils.h"
#include "component/forward.h"

namespace component::detail {
    template <UntypedComponentSignal Signal>
    class ComponentSignalHandler {
    private:
        ComponentSignalHandler<Signal>* parent;
    protected:
        virtual void handleInt (component::IterInfo& info, const Signal& signal) = 0;
    public:
        virtual ~ComponentSignalHandler () = default;
        void handle (component::IterInfo& info, const Signal& signal) {
            if constexpr (Signal::PreOrder) {
                if (parent) {
                    parent->handle(info, signal);
                }
            }

            handleInt(info, signal);

            if constexpr (!Signal::PreOrder) {
                if (parent) {
                    parent->handle(info, signal);
                }
            }
        }

        void setParent (ComponentSignalHandler<Signal>* newParent) {
            parent = newParent;
        }
    };

    template <typename T, TypedUntypedComponentSignal<T> SignalUntyped>
    class TypedComponentSignalHandler : public ComponentSignalHandler<SignalUntyped> {
    private:
        using Signal = SignalUntyped::template Signal<T>;
        std::vector<std::function<void(component::IterInfo&, const Signal&)>> handlers;
    protected:
        void handleInt (component::IterInfo& info, const SignalUntyped& signalUntyped) override {
            Signal signal = signalUntyped.template convert<T>();

            for (auto& i : handlers) {
                i(info, signal);
            }
        }
    public:
        void addHandler (std::function<void(component::IterInfo&, const Signal&)> handler) {
            handlers.emplace_back(std::move(handler));
        }
    };
}