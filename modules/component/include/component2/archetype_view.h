#pragma once

#include "util/iterable.h"

#include "archetype.h"
#include "entity2.h"

namespace phenyl::component {
    template <typename ...Args>
    class Bundle {
    private:
        Entity2 bundleEntity;
        std::tuple<Args&...> bundleComps;
    public:
        Bundle (Entity2 bundleEntity, std::tuple<Args&...> bundleComps) : bundleEntity{bundleEntity}, bundleComps{bundleComps} {}

        Entity2 entity () const noexcept {
            return bundleEntity;
        }

        const std::tuple<Args&...>& comps () const noexcept {
            return bundleComps;
        }

        template <typename T>
        T& get () const noexcept {
            return std::get<T&>(bundleComps);
        }
    };

    template <typename ...Args>
    class ArchetypeView {
    private:
        Archetype& archetype;
        ComponentManager2* manager;
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
                return {std::get<Args>(view->components)[pos + n]...};
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
        };

        class BundleIterator {
        private:
            ArchetypeView<Args...>* view = nullptr;
            std::size_t pos = 0;

        explicit BundleIterator (ArchetypeView* view, std::size_t pos = 0) : view{view}, pos{pos} {}
        public:
            using value_type = Bundle<Args...>;
            using difference_type = std::ptrdiff_t;

            BundleIterator () = default;

            value_type operator* () const {
                return {Entity2{view->archetype.entityIds[pos], view->manager}, std::get<Args>(view->components)[pos]...};
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
                return {std::get<Args>(view->components)[pos + n]...};
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
        };

        using iterator = Iterator;

        explicit ArchetypeView (Archetype& archetype, ComponentManager2* manager) : archetype{archetype}, manager{manager}, components{archetype.getComponent<Args>()...} {}

        [[nodiscard]] std::size_t size () const noexcept {
            return archetype.size();
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
    };

    // template <typename ...Args>
    // typename ArchetypeView<Args...>::Iterator operator+ (std::ptrdiff_t n, const typename ArchetypeView<Args...>::Iterator& it) noexcept {
    //     return typename ArchetypeView<Args...>::Iterator{it.view, it.pos + n};
    // }

    // template <typename ...Args>
    // typename ArchetypeView<Args...>::BundleIterator operator+ (std::ptrdiff_t n, const typename ArchetypeView<Args...>::BundleIterator& it) noexcept {
    //     return typename ArchetypeView<Args...>::BundleIterator{it.view, it.pos + n};
    // }
}