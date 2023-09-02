#pragma once

#include <array>
#include <tuple>

#include "component_set.h"
#include "utils.h"

#include "util/meta.h"

namespace component::detail {
    template <typename Signal>
    class SignalHandler {
    public:
        virtual ~SignalHandler() = default;
        virtual void handle (const Signal& event, IterInfo& info) = 0;
    };

    template <typename Signal, typename F, typename ...Args> requires meta::callable<F, void, const Signal&, IterInfo&, Args&...>
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

        void handle (const Signal& signal, IterInfo& info) override {
            auto comps = getComponents(info.id(), std::make_index_sequence<sizeof...(Args)>{});

            if (tupleAllNonNull<Args...>(comps)) {
                fn(signal, info, *(std::get<Args*>(comps))...);
            }
        }
    };


    template <typename Signal>
    class SignalHandlerList;

    class SignalHandlerListBase {
    protected:
        bool deferring{false};
        virtual void signalDeferred (ComponentManager* manager) = 0;
    public:
        virtual ~SignalHandlerListBase() = default;

        void defer () {
            deferring = true;
        }

        void deferEnd (ComponentManager* manager) {
            deferring = false;
            signalDeferred(manager);
        }
    };

    template <typename Signal>
    class SignalHandlerList : public SignalHandlerListBase {
    private:
        std::vector<std::unique_ptr<SignalHandler<Signal>>> handlers;
        std::vector<std::pair<Signal, EntityId>> deferredSignals;
    protected:
        void signalDeferred (ComponentManager* manager) override {
            for (auto& [signal, id] : deferredSignals) {
                IterInfo info{manager, id};

                for (auto& j : handlers) {
                    j->handle(signal, info);
                }
            }

            deferredSignals.clear();
        }
    public:
        template <typename ...Args>
        void handle (EntityId id, ComponentManager* manager, Args&&... args) requires std::constructible_from<Signal, Args...> {
            if (deferring) {
                deferredSignals.emplace_back(std::pair{Signal{std::forward<Args>(args)...}, id});
                return;
            }

            Signal signal{std::forward<Args>(args)...};
            IterInfo info{manager, id};

            for (auto& i : handlers) {
                i->handle(signal, info);
            }
        }

        void addHandler (std::unique_ptr<SignalHandler<Signal>> handler) {
            handlers.emplace_back(std::move(handler));
        }
    };
}