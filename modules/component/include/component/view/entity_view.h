#pragma once

#include <concepts>
#include <iterator>
#include <unordered_set>
#include <tuple>
#include <utility>

#include "component/component_new.h"


namespace component::view {
    class EntityView {
    private:
        component::EntityId entityId;
        component::ComponentManagerNew::SharedPtr compManager;

        std::unordered_set<std::size_t> bannedTypes;

        template <typename T, typename ...Ts>
        bool allValid () {
            if (bannedTypes.contains(meta::type_index<T>())) {
                return false;
            }

            if constexpr (sizeof...(Ts) > 0) {
                return allValid<Ts...>();
            } else {
                return true;
            }
        }

        template <typename T>
        util::Optional<std::tuple<T&>> getAllComps () {
            return getComponent<T>().thenMap([](auto& t) {
                return std::tuple<T&>(t);
            });
        }

        template <typename T, typename ...Ts>
        auto getAllComps () -> std::enable_if_t<0 < sizeof...(Ts), util::Optional<std::tuple<T&, Ts&...>>> {
            auto othersOpt = getAllComps<Ts...>();

            auto compOpt = getAllComps<T>();

            return othersOpt.then([&compOpt] (auto& t1) {
                return compOpt.thenMap([&t1] (auto& t2) {
                    return std::tuple_cat(t2, t1);
                });
            });
        }
        EntityView() : entityId{0, 0}, compManager(nullptr) {}
    public:
        EntityView (component::EntityId _entityId, component::ComponentManagerNew::SharedPtr  _compManager) : entityId{_entityId}, compManager{std::move(_compManager)} {}
        EntityView (component::EntityId _entityId, component::ComponentManagerNew::SharedPtr  _compManager, std::unordered_set<std::size_t> _bannedTypes) :
                entityId{_entityId}, compManager{std::move(_compManager)}, bannedTypes{std::move(_bannedTypes)} {}

        template <typename T>
        void banType () {
            bannedTypes.insert(meta::type_index<T>());
        }

        component::EntityId getId () {
            return entityId;
        }

        template <typename T>
        util::Optional<T&> getComponent () {
            if (bannedTypes.contains(meta::type_index<T>())) {

            }
            return compManager->getObjectData<T>(entityId);
        }

        template <typename T, typename ...Args>
        void addComponent (Args... args) {
            compManager->addComponent<T>(entityId, args...);
        }

        EntityView withId (component::EntityId newId) {
            return {newId, compManager, bannedTypes};
        }

        template <typename ...Ts, typename F>
        void applyFunc (F f) {
            if (allValid<Ts...>()) {
                getAllComps<Ts...>().ifPresent([&f](std::tuple<Ts&...>& tup) {
                    f(std::get<Ts&>(tup)...);
                });
            }
        }

        friend detail::EntityViewIterator;
    };

    namespace detail {
        class EntityViewIterator {
        private:
            ComponentManagerNew::SharedPtr compManager{};
            std::size_t pos{};

        public:
            EntityViewIterator () {}
            EntityViewIterator (ComponentManagerNew::SharedPtr _compManager, std::size_t startPos) : compManager{std::move(_compManager)} , pos{startPos} {}
            using iterator_category = std::random_access_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = EntityView;
            using reference = EntityView&;
            value_type operator* () const {
                return compManager->getEntityView(compManager->getComponent<component::EntityId>().orThrow()[pos]);
            }

            value_type operator[] (std::ptrdiff_t shift) const {
                return *(*this + shift);
            }

            EntityViewIterator& operator++ () {
                pos++;
                return *this;
            }

            EntityViewIterator operator++ (int) {
                EntityViewIterator other = *this;

                pos++;

                return other;
            }

            bool operator== (const EntityViewIterator& other) const {
                return pos == other.pos;
            }

            /*bool operator!= (const EntityViewIterator& other) const {
                return pos != other.pos;
            }*/

            EntityViewIterator& operator-- () {
                pos--;
                return *this;
            }

            EntityViewIterator operator-- (int) {
                EntityViewIterator other = *this;

                pos--;

                return other;
            }

            /*EntityViewIterator operator+ (std::ptrdiff_t amount) const {
                return {compManager, pos + amount};
            }*/

            friend EntityViewIterator operator+ (EntityViewIterator lhs, const std::ptrdiff_t rhs);
            friend EntityViewIterator operator+ (const std::ptrdiff_t rhs, EntityViewIterator lhs);
            friend EntityViewIterator operator- (EntityViewIterator lhs, const std::ptrdiff_t rhs);

            /*EntityViewIterator operator- (std::ptrdiff_t amount) const {
                return {compManager, pos - amount};
            }*/

            EntityViewIterator& operator+= (std::ptrdiff_t amount) {
                pos += amount;
                return *this;
            }

            EntityViewIterator& operator-= (std::ptrdiff_t amount) {
                pos -= amount;
                return *this;
            }

            bool operator< (const EntityViewIterator& other) const {
                return pos < other.pos;
            }

            bool operator<= (const EntityViewIterator& other) const {
                return !(*this > other);
            }

            bool operator> (const EntityViewIterator& other) const {
                return other < *this;
            }

            bool operator>= (const EntityViewIterator& other) const {
                return !(*this < other);
            }

            std::ptrdiff_t operator- (const EntityViewIterator& other) const {
                return (std::ptrdiff_t)pos - (std::ptrdiff_t)other.pos;
            }
        };

        inline EntityViewIterator operator+ (EntityViewIterator lhs, const std::ptrdiff_t rhs) {
            lhs += rhs;

            return lhs;
        }

        inline EntityViewIterator operator+ (const std::ptrdiff_t rhs, EntityViewIterator lhs) {
            return std::move(lhs) + rhs;
        }

        inline EntityViewIterator operator- (EntityViewIterator lhs, const std::ptrdiff_t rhs) {
            lhs -= rhs;

            return lhs;
        }
    }


}