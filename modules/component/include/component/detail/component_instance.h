#pragma once

#include <cstddef>

#include "component_vector.h"
#include "component/entity_id.h"
#include "component/entity.h"

#include "component/signals/component_update.h"

namespace phenyl::component {
    class World;
}

namespace phenyl::component::detail {
    class UntypedComponent {
    private:
        World* world;
        std::string compName;
        std::size_t typeIndex;
    protected:
        [[nodiscard]] Entity entity (EntityId id) const noexcept {
            return Entity{id, world};
        }
    public:
        explicit UntypedComponent (World* world, std::string compName, std::size_t typeIndex) : world{world}, compName{std::move(compName)}, typeIndex{typeIndex} {}
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

        virtual void deferComp (EntityId id, std::byte* comp) = 0;
        virtual void deferEnd () = 0;
    };

    template <typename T>
    class Component : public UntypedComponent {
    private:
        std::vector<std::function<void(const OnInsert<T>&, Entity)>> insertHandlers;
        std::vector<std::function<void(const OnRemove<T>&, Entity)>> removeHandlers;

        std::vector<std::pair<EntityId, T>> deferredInserts;
    public:
        Component (World* world, std::string name) : UntypedComponent(world, std::move(name), meta::type_index<T>()) {}

        std::unique_ptr<UntypedComponentVector> makeVector () override {
            return std::make_unique<ComponentVector<T>>();
        }

        void addHandler (std::function<void(const OnInsert<T>&, Entity)> handler) {
            insertHandlers.emplace_back(std::move(handler));
        }

        void addHandler (std::function<void(const OnRemove<T>&, Entity)> handler) {
            removeHandlers.emplace_back(std::move(handler));
        }

        void onInsert (EntityId id, std::byte* comp) override {
            auto e = entity(id);
            OnInsert<T> signal{comp};
            for (const auto& f : insertHandlers) {
                f(signal, e);
            }
        }

        void onRemove (EntityId id, std::byte* comp) override {
            auto e = entity(id);
            OnRemove<T> signal{comp};
            for (const auto& f : removeHandlers) {
                f(signal, e);
            }
        }

        void deferComp (EntityId id, std::byte* comp) override {
            auto* typedComp = reinterpret_cast<T*>(comp);
            deferredInserts.emplace_back(id, std::move(*typedComp));
        }

        void deferEnd () override {
            for (auto& [id, comp] : deferredInserts) {
                entity(id).insert(std::move(comp));
            }

            deferredInserts.clear();
        }
    };
}