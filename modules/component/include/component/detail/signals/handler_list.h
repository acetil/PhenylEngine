#pragma once

#include "component/forward.h"

namespace component::detail {
    class BasicManager;

    template <typename Signal>
    class SignalHandler {
    public:
        virtual ~SignalHandler() = default;
        virtual void handle (const Signal& event, BasicManager* manager, EntityId id) = 0;
    };

    class SignalHandlerListBase {
    protected:
        bool deferring{false};
        virtual void signalDeferred (detail::BasicManager* manager) = 0;
    public:
        virtual ~SignalHandlerListBase() = default;

        void defer () {
            deferring = true;
        }

        void deferEnd (detail::BasicManager* manager) {
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
        void signalDeferred (detail::BasicManager* manager) override {
            for (auto& [signal, id] : deferredSignals) {
                for (std::unique_ptr<SignalHandler<Signal>>& j : handlers) {
                    j->handle(signal, manager, id);
                }
            }

            deferredSignals.clear();
        }
    public:
        template <typename ...Args>
        void handle (EntityId id, detail::BasicManager* manager, Args&&... args) requires std::constructible_from<Signal, Args...> {
            if (deferring) {
                deferredSignals.emplace_back(std::pair{Signal{std::forward<Args>(args)...}, id});
                return;
            }

            Signal signal{std::forward<Args>(args)...};
            for (std::unique_ptr<SignalHandler<Signal>>& i : handlers) {
                i->handle(signal, manager, id);
            }
        }

        void addHandler (std::unique_ptr<SignalHandler<Signal>> handler) {
            handlers.emplace_back(std::move(handler));
        }
    };
}