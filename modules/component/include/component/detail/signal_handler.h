#pragma once

#include "component/archetype_view.h"
#include "component/entity.h"
#include "component/query.h"

namespace phenyl::component {
    class World;
}

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
    protected:
    public:
        virtual ~IHandlerVector() = default;

        virtual void handle (EntityId id, std::byte* ptr) = 0;
        virtual void defer () = 0;
        virtual void deferEnd () = 0;
    };

    template <typename Signal>
    class SignalHandlerVector : public IHandlerVector {
    private:
        World& manager;
        std::vector<std::unique_ptr<ISignalHandler<Signal>>> handlers;
        std::vector<std::pair<EntityId, Signal>> deferredSignals;
        bool isDeferred = false;

        void handleSignal (Entity entity, const Signal& signal) {
            PHENYL_DASSERT(entity.exists());
            for (const auto& i : handlers) {
                i->handle(entity, signal);
            }
        }
    public:
        explicit SignalHandlerVector (World& manager) : manager{manager} {}

        void addHandler (std::unique_ptr<ISignalHandler<Signal>> handler) {
            handlers.emplace_back(std::move(handler));
        }

        void handle (EntityId id, std::byte* signal) override {
            // Assumes creation/update/deletion of components is deferred

            auto* typedSignal = reinterpret_cast<Signal*>(signal);
            if (isDeferred) {
                deferredSignals.emplace_back(id, std::move(*typedSignal));
            } else {
                handleSignal(Entity{id, &manager}, *typedSignal);
            }
        }

        void defer () override {
            PHENYL_DASSERT(deferredSignals.empty());
            isDeferred = true;
        }

        void deferEnd () override {
            // Assumes creation/update/deletion of components/entities is deferred
            PHENYL_DASSERT(isDeferred);
            isDeferred = false;

            for (auto& [id, signal] : deferredSignals) {
                Entity entity{id, &manager};
                if (entity.exists()) {
                    handleSignal(entity, signal);
                }
            }
            deferredSignals.clear();
        }
    };
}
