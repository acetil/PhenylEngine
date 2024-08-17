#pragma once

#include "component/archetype_view.h"
#include "component/entity.h"
#include "component/query.h"

namespace phenyl::component::detail {
    template <typename Signal>
    class ISignalHandler {
    public:
        virtual ~ISignalHandler () = default;
        virtual void handle (Entity entity, const Signal& signal) const = 0;
    };

    template <typename Signal, typename ...Args>
    class SignalHandler2 : public ISignalHandler<Signal> {
    private:
        Query<Args...> query{};
        std::function<void(const Signal&, const Bundle<Args...>&)> func;

    public:
        SignalHandler2 (Query<Args...> query, std::function<void(const Signal&, const Bundle<Args...>&)> func) : query{std::move(query)}, func{std::move(func)} {}

        void handle (Entity entity, const Signal& signal) const override {
            query.entity(entity, [&] (const Bundle<Args...>& bundle) {
                func(signal, bundle);
            });
        }
    };

    class IHandlerVector {
    private:
    public:
        virtual ~IHandlerVector() = default;

        virtual void handle (Entity entity, const std::byte* ptr) const = 0;
    };

    template <typename Signal>
    class SignalHandlerVector : public IHandlerVector {
    private:
        std::vector<std::unique_ptr<ISignalHandler<Signal>>> handlers;

    public:
        void addHandler (std::unique_ptr<ISignalHandler<Signal>> handler) {
            handlers.emplace_back(std::move(handler));
        }

        void handle(Entity entity, const std::byte* signal) const override {
            auto* typedSignal = reinterpret_cast<const Signal*>(signal);
            for (const auto& i : handlers) {
                i->handle(entity, *typedSignal);
            }
        }
    };
}
