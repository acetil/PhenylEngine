#pragma once

#include <compare>
#include <map>
#include <unordered_map>

#include "util/meta.h"

#include "component/entity_id.h"
#include "detail/component_vector.h"
#include "detail/iarchetype_manager.h"

namespace phenyl::component {
    class Archetype {
    private:
        detail::IArchetypeManager& manager;
        std::map<std::size_t, std::unique_ptr<UntypedComponentVector>> components;
        std::vector<std::size_t> componentIds;
        std::vector<EntityId> entityIds;
        std::unordered_map<std::size_t, Archetype*> addArchetypes;
        std::unordered_map<std::size_t, Archetype*> removeArchetypes;

        template <typename T>
        ComponentVector<std::remove_cvref_t<T>>& getComponent () {
            PHENYL_DASSERT(components.contains(meta::type_index<std::remove_cvref_t<T>>()));
            return *components[meta::type_index<std::remove_cvref_t<T>>()];
        }

        template <typename T>
        const ComponentVector<std::remove_cvref_t<T>>& getComponent () const {
            PHENYL_DASSERT(components.contains(meta::type_index<std::remove_cvref_t<T>>()));
            return *components.at(meta::type_index<std::remove_cvref_t<T>>());
        }

        template <typename T>
        ComponentVector<std::remove_cv_t<T>>* tryGetComponent () {
            auto it = components.find(meta::type_index<std::remove_cvref_t<T>>());
            return it != components.end() ? it->second.get() : nullptr;
        }

        template <typename T>
        const ComponentVector<std::remove_cv_t<T>>* tryGetComponent () const {
            auto it = components.find(meta::type_index<std::remove_cvref_t<T>>());
            return it != components.end() ? it->second.get() : nullptr;
        }

        template <typename T>
        Archetype& getWith () {
            PHENYL_ASSERT(!has<T>());

            auto typeIndex = meta::type_index<T>();
            auto it = addArchetypes.find(typeIndex);
            if (it != addArchetypes.end()) {
                return *it->second;
            }

            std::vector<std::size_t> newComps;
            newComps.reserve(componentIds.size() + 1);
            auto cIt = newComps.begin();
            while (cIt != newComps.end() && *cIt < typeIndex) {
                newComps.emplace_back(*cIt);
                ++cIt;
            }
            newComps.emplace_back(typeIndex);
            while (cIt != newComps.end()) {
                newComps.emplace_back(*cIt);
                ++cIt;
            }

            auto* archetype = manager.findArchetype(newComps);
            if (!archetype) {
                std::map<std::size_t, std::unique_ptr<UntypedComponentVector>> newComponents;
                for (const auto& [index, vec] : components) {
                    newComponents.emplace(index, vec->makeNew());
                }
                newComponents.emplace(typeIndex, std::make_unique<ComponentVector<T>>());

                auto newArch = std::make_unique<Archetype>(manager, std::move(newComponents));
                archetype = newArch.get();
                manager.addArchetype(std::move(newArch));
            }

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

            std::vector<std::size_t> newComps;
            newComps.reserve(componentIds.size() + 1);
            auto cIt = newComps.begin();
            while (cIt != newComps.end()) {
                if (*cIt != typeIndex) {
                    newComps.emplace_back(*cIt);
                }
                ++cIt;
            }

            auto* archetype = manager.findArchetype(newComps);
            if (!archetype) {
                std::map<std::size_t, std::unique_ptr<UntypedComponentVector>> newComponents;
                for (const auto& [index, vec] : components) {
                    if (index != typeIndex) {
                        newComponents.emplace(index, vec->makeNew());
                    }
                }

                auto newArch = std::make_unique<Archetype>(manager, std::move(newComponents));
                archetype = newArch.get();
                manager.addArchetype(std::move(newArch));
            }

            removeArchetypes.emplace(typeIndex, archetype);
            archetype->addArchetypes.emplace(typeIndex, this);
            return *archetype;
        }

        template <typename T, typename ...Args>
        void initComp (Args&&... args) {
            ComponentVector<T>& comp = getComponent<T>();
            comp.emplace(std::forward<Args>(args)...);
        }

        std::size_t moveFrom (Archetype& other, std::size_t pos);

        Archetype (detail::IArchetypeManager& manager, std::map<std::size_t, std::unique_ptr<UntypedComponentVector>> components);
        Archetype (const Archetype& other, std::unique_ptr<UntypedComponentVector> compVec);

        template <typename ...Args>
        friend class ArchetypeView;
    protected:
        Archetype (detail::IArchetypeManager& manager);

        void addEntity (EntityId id);
    public:
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
            return comp ? (*comp)[pos] : nullptr;
        }

        template <typename T>
        const T* tryGet (std::size_t pos) const {
            PHENYL_DASSERT(pos < size());
            auto* comp = tryGetComponent<T>();
            return comp ? (*comp)[pos] : nullptr;
        }

        void remove (std::size_t pos);

        template <typename T, typename ...Args>
        void addComponent (std::size_t pos, Args&&... args) {
            PHENYL_DASSERT(pos < size());
            Archetype& dest = getWith<std::remove_cvref_t<T>>();

            auto newPos = dest.moveFrom(*this, pos);
            dest.initComp<std::remove_cvref_t<T>>(newPos, std::forward<Args>(args)...);
            remove(pos);
        }

        template <typename T>
        void removeComponent (std::size_t pos) {
            PHENYL_DASSERT(pos < size());
            if (!has<T>()) {
                return;
            }

            Archetype& dest = getWithout<std::remove_cvref_t<T>>();

            dest.moveFrom(*this, pos);
            remove(pos);
        }

        const std::vector<std::size_t>& getComponentIds () const noexcept {
            return componentIds;
        }
    };

    template <typename ...Args>
    class ArchetypeView {
    private:
        Archetype& archetype;
        std::tuple<ComponentVector<Args>*...> components;

    public:
        class Iterator {
        private:
            ArchetypeView<Args...>* view = nullptr;
            std::size_t pos = 0;

        explicit Iterator (ArchetypeView* view, std::size_t pos = 0) : view{view}, pos{pos} {}
        public:
            using value_type = std::tuple<Args&...>;
            using difference_type = std::ptrdiff_t;

            Iterator () = default;

            value_type operator* () const {
                return {std::get<Args>(view->components)[pos]...};
            }

            Iterator& operator++ () {
                pos++;
                return *this;
            }
            Iterator operator++ (int) {
                auto copy = *this;
                ++this;
                return copy;
            }

            Iterator& operator-- () {
                pos--;
                return *this;
            }
            Iterator operator-- (int) {
                auto copy = *this;
                --this;
                return copy;
            }

            Iterator& operator+= (difference_type n) {
                pos += n;
                return *this;
            }
            Iterator operator+ (difference_type n) const noexcept {
                auto copy = *this;
                copy += n;
                return copy;
            }

            Iterator& operator-= (difference_type n) {
                pos -= n;
                return *this;
            }
            Iterator operator- (difference_type n) const noexcept {
                auto copy = *this;
                copy -= n;
                return copy;
            }

            difference_type operator- (const Iterator& other) const noexcept {
                return static_cast<difference_type>(pos) - static_cast<difference_type>(other.pos);
            }

            value_type operator[] (difference_type n) const {
                return {std::get<Args>(view->components)[pos + n]...};
            }

            bool operator== (const Iterator& other) const noexcept {
                return view == other.view && pos == other.pos;
            }

            std::strong_ordering operator<=> (const Iterator& other) const noexcept {
                return pos <=> other.pos;
            }

            friend Iterator operator+ (difference_type n, const Iterator& it) noexcept;
        };

        using iterator = Iterator;

        explicit ArchetypeView (Archetype& archetype) : archetype{archetype}, components{archetype.getComponent<Args>()...} {}

        [[nodiscard]] std::size_t size () const noexcept {
            return archetype.size();
        }

        iterator begin () {
            return Iterator{this};
        }

        iterator end () {
            return Iterator{this, size()};
        }
    };

    template <typename ...Args>
    typename ArchetypeView<Args...>::Iterator operator+ (typename ArchetypeView<Args...>::Iterator::difference_type n, const typename ArchetypeView<Args...>::Iterator& it) noexcept {
        return typename ArchetypeView<Args...>::Iterator{it.view, it.pos + n};
    }

    class EmptyArchetype : public Archetype {
    public:
        explicit EmptyArchetype (detail::IArchetypeManager& manager) : Archetype{manager} {}

        void add (EntityId id) {
            addEntity(id);
        }
    };
}
   