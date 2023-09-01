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
            auto comps = getComponents(idFromInfo(info), std::make_index_sequence<sizeof...(Args)>{});

            if (tupleAllNonNull<Args...>(comps)) {
                fn(signal, info, *(std::get<Args*>(comps))...);
            }
        }
    };


    template <typename Signal>
    class SignalHandlerList;

    class SignalHandlerListBase {
    public:
        virtual ~SignalHandlerListBase() = default;
    };

    template <typename Signal>
    class SignalHandlerList : public SignalHandlerListBase {
    private:
        std::vector<std::unique_ptr<SignalHandler<Signal>>> handlers;

    public:
        template <typename ...Args>
        void handle (IterInfo& info, Args&&... args) requires std::constructible_from<Signal, Args...> {
            Signal signal{std::forward<Args>(args)...};

            for (auto& i : handlers) {
                i->handle(signal, info);
            }
        }

        void addHandler (std::unique_ptr<SignalHandler<Signal>> handler) {
            handlers.emplace_back(std::move(handler));
        }
    };
}