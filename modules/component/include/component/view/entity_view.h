#pragma once

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

        template <typename ...Ts>
        bool allValid () {
            for (auto& x : meta::type_index<Ts...>()) {
                if (bannedTypes.contains(x)) {
                    return false;
                }
            }
            return true;
        }

        template <typename T>
        util::Optional<std::tuple<T&>> getAllComps () {
            return getComponent<T>().thenMap([](auto& t) {
                return std::tuple<T&>(t);
            });
        }

        template <typename T, typename ...Ts>
        util::Optional<std::tuple<T&, Ts&...>> getAllComps () {
            auto othersOpt = getAllComps<Ts...>();

            auto compOpt = getAllComps<T>();

            return othersOpt.thenMap([&compOpt] (auto& t1) {
                return compOpt.thenMap([&t1] (auto& t2) {
                    return std::tuple_cat(t1, t2);
                });
            });
        }

    public:
        EntityView (component::EntityId _entityId, component::ComponentManagerNew::SharedPtr  _compManager) : entityId{_entityId}, compManager{std::move(_compManager)} {}
        EntityView (component::EntityId _entityId, component::ComponentManagerNew::SharedPtr  _compManager, std::unordered_set<std::size_t> _bannedTypes) :
                entityId{_entityId}, compManager{std::move(_compManager)}, bannedTypes{std::move(_bannedTypes)} {}

        template <typename T>
        void banType () {
            bannedTypes.insert(meta::type_index<T>::val());
        }

        component::EntityId getId () {
            return entityId;
        }

        template <typename T>
        util::Optional<T&> getComponent () {
            if (bannedTypes.contains(meta::type_index<T>::val())) {

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

        template <typename F, typename ...Ts, typename ...Args>
        void applyFunc (F f, Args... args) {
            if (allValid<Ts...>()) {
                getAllComps<Ts...>().ifPresent([&f, &args...](auto& tup) {
                    f(std::get<meta::meta_range<sizeof...(Ts)>>(tup), args...);
                });
            }
        }
    };
}