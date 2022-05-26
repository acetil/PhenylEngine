#include <cstddef>

#pragma once

namespace common {
    namespace detail {
        // TODO: mutex
        struct ScopeControlBlock {
            std::size_t owners{0};
            std::size_t observers{0};

            [[nodiscard]] bool isScopeAlive () const {
                return owners != 0;
            }

            [[nodiscard]] bool shouldDelete () const {
                return owners == 0 && observers == 0;
            }

            void addOwner () {
                owners++;
            }

            void addObserver () {
                observers++;
            }

            void removeOwner () {
                owners--;
            }

            void removeObserver () {
                observers--;
            }
        };
    }

    class ScopeObserver;

    class Scope {
    private:
        detail::ScopeControlBlock* controlBlock{nullptr};
        void removeControlBlock () {
            if (!controlBlock) {
                return;
            }

            controlBlock->removeOwner();

            if (controlBlock->shouldDelete()) {
                delete controlBlock;
            }
        }
    public:
        Scope () = default;
        Scope (const Scope& other) {
            controlBlock = other.controlBlock;

            if (controlBlock) {
                controlBlock->addOwner();
            }
        }

        Scope (Scope&& other) noexcept {
            controlBlock = other.controlBlock;
            other.controlBlock = nullptr;
        }

        Scope& operator= (const Scope& other) {
            if (&other == this) {
                return *this;
            }

            removeControlBlock();

            controlBlock = other.controlBlock;
            if (controlBlock) {
                controlBlock->addOwner();
            }

            return *this;
        }

        Scope& operator= (Scope&& other) noexcept {
            removeControlBlock();

            controlBlock = other.controlBlock;
            other.controlBlock = nullptr;

            return *this;
        }

        ~Scope() {
            removeControlBlock();
        }

        void initScope () {
            removeControlBlock();

            controlBlock = new detail::ScopeControlBlock;
            controlBlock->addOwner();
        }

        operator bool () {
            return controlBlock;
        }

        friend class ScopeObserver;
    };

    class ScopeObserver {
    private:
        detail::ScopeControlBlock* controlBlock{nullptr};
        void incrementObserverCount () {
            if (controlBlock) {
                controlBlock->addObserver();
            }
        }

        void decrementObserverCount () {
            if (!controlBlock) {
                return;
            }

            controlBlock->removeObserver();

            if (controlBlock->shouldDelete()) {
                delete controlBlock;
            }
        }
    public:
        ScopeObserver () = default;
        ScopeObserver (const Scope& scope) {
            controlBlock = scope.controlBlock;
            incrementObserverCount();
        }

        ScopeObserver (const ScopeObserver& other) {
            controlBlock = other.controlBlock;
            incrementObserverCount();
        }
        ScopeObserver (ScopeObserver&& other) noexcept {
            controlBlock = other.controlBlock;
            other.controlBlock = nullptr;
        }

        ScopeObserver& operator= (const Scope& scope) {
            decrementObserverCount();
            controlBlock = scope.controlBlock;
            incrementObserverCount();
            return *this;
        }
        ScopeObserver& operator= (const ScopeObserver& other) {
            if (&other == this) {
                return *this;
            }
            decrementObserverCount();
            controlBlock = other.controlBlock;
            incrementObserverCount();

            return *this;
        }
        ScopeObserver& operator= (ScopeObserver&& other) noexcept {
            decrementObserverCount();
            controlBlock = other.controlBlock;
            other.controlBlock = nullptr;

            return *this;
        }

        ~ScopeObserver() {
            decrementObserverCount();
        }

        operator bool () const {
            return controlBlock && controlBlock->isScopeAlive();
        }
    };
}