#pragma once

#include <cstddef>

#include "component_vector.h"
#include "component/entity_id.h"
#include "component2/entity2.h"

#include "component2/signals/component_update.h"

namespace phenyl::component {
    class ComponentManager2;
}

namespace phenyl::component::detail {
    class UntypedComponent {
    private:
        ComponentManager2* manager;
        std::string compName;
        std::size_t typeIndex;
    protected:
        [[nodiscard]] Entity2 entity (EntityId id) const noexcept {
            return Entity2{id, manager};
        }
    public:
        explicit UntypedComponent (ComponentManager2* manager, std::string compName, std::size_t typeIndex) : manager{manager}, compName{std::move(compName)}, typeIndex{typeIndex} {}
        virtual ~UntypedComponent() = default;

        [[nodiscard]] std::size_t type () const noexcept {
            return typeIndex;
        }

        const std::string& name () const noexcept {
            return compName;
        }

        virtual std::unique_ptr<UntypedComponentVector> makeVector () = 0;
        virtual void onInsert (EntityId id, std::byte* comp) = 0;
        virtual void onRemove (EntityId id, std::byte* comp) = 0;
    };

    template <typename T>
    class Component : public UntypedComponent {
    private:
        std::vector<std::function<void(const OnInsert2<T>&, Entity2)>> insertHandlers;
        std::vector<std::function<void(const OnRemove2<T>&, Entity2)>> removeHandlers;
    public:
        Component (ComponentManager2* manager, std::string name) : UntypedComponent(manager, std::move(name), meta::type_index<T>()) {}

        std::unique_ptr<UntypedComponentVector> makeVector () override {
            return std::make_unique<ComponentVector<T>>();
        }

        void addHandler (std::function<void(const OnInsert2<T>&, Entity2)> handler) {
            insertHandlers.emplace_back(std::move(handler));
        }

        void addHandler (std::function<void(const OnRemove2<T>&, Entity2)> handler) {
            removeHandlers.emplace_back(std::move(handler));
        }

        void onInsert (EntityId id, std::byte* comp) override {
            auto e = entity(id);
            OnInsert2<T> signal{comp};
            for (const auto& f : insertHandlers) {
                f(signal, e);
            }
        }

        void onRemove (EntityId id, std::byte* comp) override {
            auto e = entity(id);
            OnRemove2<T> signal{comp};
            for (const auto& f : removeHandlers) {
                f(signal, e);
            }
        }
    };
}