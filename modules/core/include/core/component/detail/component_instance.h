#pragma once

#include "component_vector.h"
#include "core/entity.h"
#include "core/entity_id.h"
#include "core/signals/component_update.h"

#include <cstddef>

namespace phenyl::core {
class World;
}

namespace phenyl::core::detail {
class UntypedComponent {
public:
    explicit UntypedComponent (World* world, std::string compName, std::size_t typeIndex) :
        m_world{world},
        m_name{std::move(compName)},
        m_type{typeIndex} {}

    virtual ~UntypedComponent () = default;

    [[nodiscard]] std::size_t type () const noexcept {
        return m_type;
    }

    [[nodiscard]] const std::string& name () const noexcept {
        return m_name;
    }

    virtual std::unique_ptr<UntypedComponentVector> makeVector () = 0;
    virtual void onInsert (EntityId id, std::byte* comp) = 0;
    virtual void onRemove (EntityId id, std::byte* comp) = 0;

    virtual void deferComp (EntityId id, std::byte* comp) = 0;
    virtual void deferErase (EntityId id) = 0;
    virtual void deferEnd () = 0;

protected:
    [[nodiscard]] Entity entity (EntityId id) const noexcept {
        return Entity{id, m_world};
    }

private:
    World* m_world;
    std::string m_name;
    std::size_t m_type;
};

template<typename T>
class Component : public UntypedComponent {
public:
    Component (World* world, std::string name) : UntypedComponent(world, std::move(name), meta::type_index<T>()) {}

    std::unique_ptr<UntypedComponentVector> makeVector () override {
        return std::make_unique<ComponentVector<T>>();
    }

    void addHandler (std::function<void(const OnInsert<T>&, Entity)> handler) {
        m_insertHandlers.emplace_back(std::move(handler));
    }

    void addHandler (std::function<void(const OnRemove<T>&, Entity)> handler) {
        m_removeHandlers.emplace_back(std::move(handler));
    }

    void onInsert (EntityId id, std::byte* comp) override {
        auto e = entity(id);
        OnInsert<T> signal{comp};
        for (const auto& f : m_insertHandlers) {
            f(signal, e);
        }
    }

    void onRemove (EntityId id, std::byte* comp) override {
        auto e = entity(id);
        OnRemove<T> signal{comp};
        for (const auto& f : m_removeHandlers) {
            f(signal, e);
        }
    }

    void deferComp (EntityId id, std::byte* comp) override {
        auto* typedComp = reinterpret_cast<T*>(comp);
        m_deferredInserts.emplace_back(id, std::move(*typedComp));
    }

    void deferErase (EntityId id) override {
        m_deferredErases.emplace_back(id);
    }

    void deferEnd () override {
        for (auto& [id, comp] : m_deferredInserts) {
            entity(id).insert(std::move(comp));
        }

        for (auto id : m_deferredErases) {
            entity(id).template erase<T>();
        }

        m_deferredInserts.clear();
        m_deferredErases.clear();
    }

private:
    std::vector<std::function<void(const OnInsert<T>&, Entity)>> m_insertHandlers;
    std::vector<std::function<void(const OnRemove<T>&, Entity)>> m_removeHandlers;

    std::vector<std::pair<EntityId, T>> m_deferredInserts;
    std::vector<EntityId> m_deferredErases;
};
} // namespace phenyl::core::detail
