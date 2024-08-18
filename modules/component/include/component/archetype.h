#pragma once

#include <compare>
#include <map>
#include <unordered_map>

#include "util/meta.h"

#include "entity_id.h"
#include "detail/archetype_key.h"
#include "detail/component_vector.h"
#include "detail/iarchetype_manager.h"
#include "detail/prefab_factory.h"

namespace phenyl::component {
    class Archetype {
    private:
        detail::IArchetypeManager& manager;
        detail::ArchetypeKey key;

        std::map<std::size_t, std::unique_ptr<UntypedComponentVector>> components;
        std::vector<EntityId> entityIds;

        std::unordered_map<std::size_t, Archetype*> addArchetypes;
        std::unordered_map<std::size_t, Archetype*> removeArchetypes;

        template <typename T>
        ComponentVector<std::remove_cvref_t<T>>& getComponent () {
            PHENYL_DASSERT(components.contains(meta::type_index<std::remove_cvref_t<T>>()));
            return static_cast<ComponentVector<std::remove_cvref_t<T>>&>(*components[meta::type_index<std::remove_cvref_t<T>>()]);
        }

        template <typename T>
        const ComponentVector<std::remove_cvref_t<T>>& getComponent () const {
            PHENYL_DASSERT(components.contains(meta::type_index<std::remove_cvref_t<T>>()));
            return static_cast<ComponentVector<std::remove_cvref_t<T>>&>(*components.at(meta::type_index<std::remove_cvref_t<T>>()));
        }

        template <typename T>
        ComponentVector<std::remove_cvref_t<T>>* tryGetComponent () {
            auto it = components.find(meta::type_index<std::remove_cvref_t<T>>());
            return it != components.end() ? static_cast<ComponentVector<std::remove_cvref_t<T>>*>(it->second.get()) : nullptr;
        }

        template <typename T>
        const ComponentVector<std::remove_cv_t<T>>* tryGetComponent () const {
            auto it = components.find(meta::type_index<std::remove_cvref_t<T>>());
            return it != components.end() ? static_cast<ComponentVector<std::remove_cvref_t<T>>*>(it->second.get()) : nullptr;
        }

        template <typename T>
        Archetype& getWith () {
            PHENYL_ASSERT(!has<T>());

            auto typeIndex = meta::type_index<T>();
            auto it = addArchetypes.find(typeIndex);
            if (it != addArchetypes.end()) {
                return *it->second;
            }

            auto* archetype = manager.findArchetype(key.with<T>());
            addArchetypes.emplace(typeIndex, archetype);
            archetype->removeArchetypes.emplace(typeIndex, this);
            return *archetype;
        }

        template <typename T>
        Archetype& getWithout () {
            PHENYL_ASSERT(has<T>());

            auto typeIndex = meta::type_index<T>();
            auto it = removeArchetypes.find(typeIndex);
            if (it != removeArchetypes.end()) {
                return *it->second;
            }

            auto* archetype = manager.findArchetype(key.without<T>());
            removeArchetypes.emplace(typeIndex, archetype);
            archetype->addArchetypes.emplace(typeIndex, this);
            return *archetype;
        }

        template <typename T, typename ...Args>
        void initComp (Args&&... args) {
            ComponentVector<T>& comp = getComponent<T>();
            auto* ptr = comp.emplace(std::forward<Args>(args)...);

            manager.onComponentInsert(entityIds.back(), meta::type_index<T>(), reinterpret_cast<std::byte*>(ptr));
        }

        std::size_t moveFrom (Archetype& other, std::size_t pos);
        void instantiateInto (const detail::PrefabFactories& factories, std::size_t pos);

        template <typename ...Args>
        friend class ArchetypeView;
        friend class World;
    protected:
        Archetype (detail::IArchetypeManager& manager);

        std::size_t addEntity (EntityId id);
    public:
        Archetype (detail::IArchetypeManager& manager, std::map<std::size_t, std::unique_ptr<UntypedComponentVector>> components);

        bool hasUntyped (std::size_t typeIndex) const noexcept {
            return components.contains(typeIndex);
        }

        template <typename T>
        bool has () const noexcept {
            return hasUntyped(meta::type_index<std::remove_cvref_t<T>>());
        }

        template <typename ...Args, std::size_t N = 0>
        bool hasAll () const noexcept {
            if constexpr (N == sizeof...(Args)) {
                return true;
            } else {
                return has<std::tuple_element_t<N, std::tuple<Args...>>>();
            }
        }

        std::size_t size () const noexcept {
            return entityIds.size();
        }

        template <typename T>
        T& get (std::size_t pos) {
            PHENYL_DASSERT(pos < size());
            return getComponent<T>()[pos];
        }

        template <typename T>
        const T& get (std::size_t pos) const {
            PHENYL_DASSERT(pos < size());
            return getComponent<T>()[pos];
        }

        template <typename T>
        T* tryGet (std::size_t pos) {
            PHENYL_DASSERT(pos < size());
            auto* comp = tryGetComponent<T>();
            return comp ? &(*comp)[pos] : nullptr;
        }

        template <typename T>
        const T* tryGet (std::size_t pos) const {
            PHENYL_DASSERT(pos < size());
            auto* comp = tryGetComponent<T>();
            return comp ? &(*comp)[pos] : nullptr;
        }

        void remove (std::size_t pos);

        template <typename T, typename ...Args>
        void addComponent (std::size_t pos, Args&&... args) {
            PHENYL_DASSERT(pos < size());
            Archetype& dest = getWith<std::remove_cvref_t<T>>();
            dest.moveFrom(*this, pos);
            remove(pos);
            dest.initComp<std::remove_cvref_t<T>>(std::forward<Args>(args)...);
        }

        template <typename T>
        void removeComponent (std::size_t pos) {
            PHENYL_DASSERT(pos < size());
            if (!has<T>()) {
                return;
            }

            manager.onComponentRemove(entityIds[pos], meta::type_index<T>(), reinterpret_cast<std::byte*>(&getComponent<T>()[pos]));
            Archetype& dest = getWithout<std::remove_cvref_t<T>>();

            dest.moveFrom(*this, pos);
            remove(pos);
        }

        void clear ();

        const detail::ArchetypeKey& getKey () const noexcept {
            return key;
        }

        void instantiatePrefab (const detail::PrefabFactories& factories, std::size_t pos);
    };

    class EmptyArchetype : public Archetype {
    public:
        explicit EmptyArchetype (detail::IArchetypeManager& manager) : Archetype{manager} {}

        void add (EntityId id) {
            addEntity(id);
        }
    };
}
   