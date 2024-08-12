#pragma once

#include "component2/archetype_view.h"
#include "component2/entity2.h"
#include "component2/query.h"

namespace phenyl::component::detail {
    template <typename Signal>
    class ISignalHandler {
    public:
        virtual ~ISignalHandler () = default;
        virtual void handle (Entity2 entity, const Signal& signal) const = 0;
    };

    template <typename Signal, typename ...Args>
    class SignalHandler : public ISignalHandler<Signal> {
    private:
        Query2<Args...> query{};
        std::function<void(const Signal&, const Bundle<Args...>&)> func;

    public:
        SignalHandler (Query2<Args...> query, std::function<void(const Signal&, const Bundle<Args...>&)> func) : query{std::move(query)}, func{std::move(func)} {}

        void handle (Entity2 entity, const Signal& signal) const override {
            query.entity(entity, [&] (const Bundle<Args...>& bundle) {
                func(signal, bundle);
            });
        }
    };

    class IHandlerVector {
    private:
    public:
        virtual ~IHandlerVector() = default;

        virtual void handle (Entity2 entity, const std::byte* ptr) const;
    };

    template <typename Signal>
    class SignalHandlerVector : public IHandlerVector {
    private:
        std::vector<std::unique_ptr<ISignalHandler<Signal>>> handlers;

    public:
        void addHandler (std::unique_ptr<ISignalHandler<Signal>> handler) {
            handlers.emplace_back(std::move(handler));
        }

        void handle(Entity2 entity, const std::byte* signal) const override {
            auto* typedSignal = static_cast<const Signal*>(signal);
            for (const auto& i : handlers) {
                i->handle(entity, *typedSignal);
            }
        }
    };
}
