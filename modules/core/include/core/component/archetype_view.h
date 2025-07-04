#pragma once

#include "archetype.h"
#include "core/entity.h"
#include "util/iterable.h"

namespace phenyl::core {
template <typename... Args>
class Bundle {
public:
    Bundle (Entity bundleEntity, std::tuple<Args&...> bundleComps) :
        bundleEntity{bundleEntity},
        bundleComps{bundleComps} {}

    Entity entity () const noexcept {
        return bundleEntity;
    }

    const std::tuple<Args&...>& comps () const noexcept {
        return bundleComps;
    }

    template <typename T>
    T& get () const noexcept {
        return std::get<T&>(bundleComps);
    }

    template <typename... Args2>
    Bundle<Args2...> subset () const noexcept {
        return Bundle<Args2...>{bundleEntity, std::tuple<Args2&...>{get<Args2&>()...}};
    }

private:
    Entity bundleEntity;
    std::tuple<Args&...> bundleComps;
};

template <typename... Args>
class ArchetypeView {
public:
    class Iterator {
    public:
        using value_type = std::tuple<Args&...>;
        using difference_type = std::ptrdiff_t;

        Iterator () = default;

        value_type operator* () const {
            return {view->get<Args>()[pos]...};
        }

        Iterator& operator++ () {
            pos++;
            return *this;
        }

        Iterator operator++ (int) {
            auto copy = *this;
            ++*this;
            return copy;
        }

        Iterator& operator-- () {
            pos--;
            return *this;
        }

        Iterator operator-- (int) {
            auto copy = *this;
            --*this;
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
            return value_type{view->get<Args>()[pos + n]...};
        }

        bool operator== (const Iterator& other) const noexcept {
            return view == other.view && pos == other.pos;
        }

        std::strong_ordering operator<=> (const Iterator& other) const noexcept {
            return pos <=> other.pos;
        }

        friend Iterator operator+ (std::ptrdiff_t n, const Iterator& it) noexcept {
            return Iterator{it.view, it.pos + n};
        }

    private:
        ArchetypeView<Args...>* view = nullptr;
        std::size_t pos = 0;

        explicit Iterator (ArchetypeView* view, std::size_t pos = 0) : view{view}, pos{pos} {}

        friend ArchetypeView<Args...>;
    };

    class BundleIterator {
    public:
        using value_type = Bundle<Args...>;
        using difference_type = std::ptrdiff_t;

        BundleIterator () = default;

        value_type operator* () const {
            return value_type{Entity{view->archetype.m_entityIds[pos], view->manager},
              std::tuple<Args&...>{view->get<Args>()[pos]...}};
        }

        BundleIterator& operator++ () {
            pos++;
            return *this;
        }

        BundleIterator operator++ (int) {
            auto copy = *this;
            ++*this;
            return copy;
        }

        BundleIterator& operator-- () {
            pos--;
            return *this;
        }

        BundleIterator operator-- (int) {
            auto copy = *this;
            --*this;
            return copy;
        }

        BundleIterator& operator+= (difference_type n) {
            pos += n;
            return *this;
        }

        BundleIterator operator+ (difference_type n) const noexcept {
            auto copy = *this;
            copy += n;
            return copy;
        }

        BundleIterator& operator-= (difference_type n) {
            pos -= n;
            return *this;
        }

        BundleIterator operator- (difference_type n) const noexcept {
            auto copy = *this;
            copy -= n;
            return copy;
        }

        difference_type operator- (const BundleIterator& other) const noexcept {
            return static_cast<difference_type>(pos) - static_cast<difference_type>(other.pos);
        }

        value_type operator[] (difference_type n) const {
            return value_type{Entity{view->archetype.m_entityIds[pos + n], view->manager},
              std::tuple<Args&...>{view->get<Args>()[pos + n]...}};
        }

        bool operator== (const BundleIterator& other) const noexcept {
            return view == other.view && pos == other.pos;
        }

        std::strong_ordering operator<=> (const BundleIterator& other) const noexcept {
            return pos <=> other.pos;
        }

        friend BundleIterator operator+ (std::ptrdiff_t n, const BundleIterator& it) noexcept {
            return BundleIterator{it.view, it.pos + n};
        }

    private:
        ArchetypeView<Args...>* view = nullptr;
        std::size_t pos = 0;

        explicit BundleIterator (ArchetypeView* view, std::size_t pos = 0) : view{view}, pos{pos} {}

        friend ArchetypeView<Args...>;
    };

    using iterator = Iterator;

    explicit ArchetypeView (Archetype& archetype, World* manager) :
        archetype{archetype},
        manager{manager},
        components{archetype.getComponentView<std::remove_reference_t<Args>>(meta::TypeIndex::Get<Args>())...} {}

    [[nodiscard]] std::size_t size () const noexcept {
        return archetype.size();
    }

    Bundle<Args...> bundle (std::size_t pos) {
        return {Entity{archetype.m_entityIds[pos], manager}, std::tuple<Args&...>{get<Args>()[pos]...}};
    }

    iterator begin () {
        return Iterator{this};
    }

    iterator end () {
        return Iterator{this, size()};
    }

    util::Iterable<BundleIterator> bundles () {
        return {BundleIterator{this}, BundleIterator{this, size()}};
    }

private:
    Archetype& archetype;
    World* manager;
    std::tuple<ComponentView<std::remove_reference_t<Args>>...> components;

    template <typename T>
    ComponentView<T>& get () {
        return std::get<ComponentView<std::remove_reference_t<T>>>(components);
    }

    template <typename T>
    const ComponentView<T>& get () const {
        return std::get<ComponentView<std::remove_reference_t<T>>>(components);
    }
};
} // namespace phenyl::core
