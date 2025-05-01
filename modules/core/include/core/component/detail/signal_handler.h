#pragma once

#include "core/component/archetype_view.h"
#include "core/entity.h"
#include "core/component/query.h"

namespace phenyl::core {
    class World;
}

namespace phenyl::core::detail {
    template <typename Signal>
    class ISignalHandler {
    public:
        virtual ~ISignalHandler () = default;
        virtual void handle (Entity entity, const Signal& signal) const = 0;
    };

    template <typename Signal, typename ...Args>
    class SignalHandler2 : public ISignalHandler<Signal> {
    public:
        SignalHandler2 (Query<Args...> query, std::function<void(const Signal&, const Bundle<Args...>&)> func) : m_query{std::move(query)}, m_func{std::move(func)} {}

        void handle (Entity entity, const Signal& signal) const override {
            m_query.entity(entity, [&] (const Bundle<Args...>& bundle) {
                m_func(signal, bundle);
            });
        }

    private:
        Query<Args...> m_query{};
        std::function<void(const Signal&, const Bundle<Args...>&)> m_func;
    };

    class IHandlerVector {
    public:
        virtual ~IHandlerVector() = default;

        virtual void handle (EntityId id, std::byte* ptr) = 0;
        virtual void defer () = 0;
        virtual void deferEnd () = 0;
    };

    template <typename Signal>
    class SignalHandlerVector : public IHandlerVector {
    public:
        explicit SignalHandlerVector (World& manager) : m_manager{manager} {}

        void addHandler (std::unique_ptr<ISignalHandler<Signal>> handler) {
            m_handlers.emplace_back(std::move(handler));
        }

        void handle (EntityId id, std::byte* signal) override {
            // Assumes creation/update/deletion of components is deferred

            auto* typedSignal = reinterpret_cast<Signal*>(signal);
            if (m_isDeferred) {
                m_deferredSignals.emplace_back(id, std::move(*typedSignal));
            } else {
                handleSignal(Entity{id, &m_manager}, *typedSignal);
            }
        }

        void defer () override {
            PHENYL_DASSERT(m_deferredSignals.empty());
            m_isDeferred = true;
        }

        void deferEnd () override {
            // Assumes creation/update/deletion of components/entities is deferred
            PHENYL_DASSERT(m_isDeferred);
            m_isDeferred = false;

            for (auto& [id, signal] : m_deferredSignals) {
                Entity entity{id, &m_manager};
                if (entity.exists()) {
                    handleSignal(entity, signal);
                }
            }
            m_deferredSignals.clear();
        }

    private:
        World& m_manager;
        std::vector<std::unique_ptr<ISignalHandler<Signal>>> m_handlers;
        std::vector<std::pair<EntityId, Signal>> m_deferredSignals;
        bool m_isDeferred = false;

        void handleSignal (Entity entity, const Signal& signal) {
            PHENYL_DASSERT(entity.exists());
            for (const auto& i : m_handlers) {
                i->handle(entity, signal);
            }
        }
    };
}
