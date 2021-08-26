#include <unordered_map>
#include <string>
#include <string.h>
#include <utility>
#include <vector>
#include <type_traits>
#include <stdint.h>
#include <memory>

#include "event/events/object_id_swap.h"

#include <cmath>
#include <graphics/graphics_new_include.h>
#include <physics/collision_component.h>
#include "event/event.h"
#include "main_component.h"
#include "util/meta.h"
#include "rotation_component.h"
#include "game/entity/entity_type.h"
#include "util/smart_help.h"

#ifndef MAX_COMPONENT_ENTITIES
#define MAX_COMPONENT_ENTITIES 256
#endif
#ifndef COMPONENT_H
#define COMPONENT_H
#if MAX_COMPONENT_ENTITIES <= 256
namespace component {
    typedef uint16_t id_type_t;
}
#else
namespace component {
    typedef uint16_t id_type_t;
}
#endif
namespace game {
#ifndef ENTITY_H
        class AbstractEntity;
#endif
#ifndef CONTROLLER_H
        class EntityController;
#endif
};
namespace view {
    template<typename ...Args>
    class ViewCore;
}
namespace component {
    template <typename ...Args>
    class ComponentManagerImpl : public util::SmartHelper<ComponentManagerImpl<Args...>> {
        using FirstType = meta::get_first<Args...>;
    private:
        std::tuple<meta::add_pointer<Args>...> ptrTuple;
        id_type_t numEntities;
        id_type_t maxNumEntities;
        event::EventBus::SharedPtr eventBus;
    public:
        //template <int N, typename T>
        //void initData () {

        //}
        using type_list = meta::type_list_wrapper<Args...>;

        static constexpr const int num_comps = sizeof...(Args);

        template <int N = 0, std::enable_if_t<N < sizeof...(Args), int> = 0>
        void initData () {
            using T = typename std::remove_pointer<typename std::remove_reference<decltype(std::get<N>(ptrTuple))>::type>::type;
            std::get<N>(ptrTuple) = new T[maxNumEntities];
            initData<N + 1>();
        }
        template <int N, std::enable_if_t<N >= sizeof...(Args), int> = 0>
        void initData () {

        }
        template <int N, std::enable_if_t<N >= sizeof...(Args), int> = 0>
        void destroyData () {

        }
        template <int N = 0, std::enable_if_t<N < sizeof...(Args), int> = 0>
        void destroyData () {
            if constexpr (N == 0 && std::is_pointer<FirstType>::value) {
                for (int i = 0; i < numEntities; i++) {
                    delete std::get<N>(ptrTuple)[i];
                }
            }
            delete[] std::get<N>(ptrTuple);
            destroyData<N + 1>();
        }

        explicit ComponentManagerImpl(id_type_t maxEntities, event::EventBus::SharedPtr bus) {
            maxNumEntities = maxEntities;
            numEntities = 0;
            eventBus = std::move(bus);
            initData();
        }
        ~ComponentManagerImpl () {
            destroyData();
        }

        template <typename T>
        T* getComponent () {
            return std::get<meta::add_pointer<T>>(ptrTuple);
        }
        template <int N>
        decltype(std::get<N>(ptrTuple)) getComponent () {
            return std::get<N>(ptrTuple);
        }
        template <typename T>
        T& getObjectData (int entityId) {
            T* temp = std::get<meta::add_pointer<T>>(ptrTuple) + entityId;
            //static_assert(std::is_same<decltype(temp2),T&>::value, "Typecast failed!");
            return reinterpret_cast<T&>(*temp);
        }
        template <typename T>
        T* getObjectDataPtr (int entityId) {
            return std::get<meta::add_pointer<T>>(ptrTuple) + entityId;
        }
        id_type_t getNumObjects () {
            return numEntities;
        }
        id_type_t addObject (FirstType obj) {
            logging::log(LEVEL_DEBUG, "New object! Num objects: {}", numEntities + 1);
            std::get<0>(ptrTuple)[numEntities] = obj;
            return numEntities++;
        }
        id_type_t addObject () {
            logging::log(LEVEL_DEBUG, "New object! Num objects: {}", numEntities + 1);
            return numEntities++;
        }
        template <int N = 0, std::enable_if_t<N < sizeof...(Args), int> = 0>
        void swapObjects (id_type_t oldId, id_type_t newId) {
            //std::get<N>(ptrTuple)[newId] = std::get<N>(ptrTuple)[oldId];
            memcpy(std::get<N>(ptrTuple) + newId, std::get<N>(ptrTuple) + oldId, sizeof(decltype(*std::get<N>(ptrTuple))));
            swapObjects<N + 1>(oldId, newId);
        }
        template <int N = 0, std::enable_if_t<N >= sizeof...(Args), int> = 0>
        void swapObjects (id_type_t oldId, id_type_t newId) {

        }
        void removeObject (id_type_t entityId) {
            logging::log(LEVEL_DEBUG, "Removing entity {}!", entityId);
            if (entityId >= numEntities) {
                logging::log(LEVEL_ERROR, "Attempted to remove object id {}, only {} object components!", entityId, numEntities);
                return;
            }
            id_type_t oldId = --numEntities;
            if constexpr (std::is_pointer<FirstType>::value) {
                delete std::get<0>(ptrTuple)[entityId];
            }
            swapObjects<>(oldId, entityId);
            eventBus->raiseEvent(event::ObjectIdSwapEvent<meta::remove_pointer<FirstType>>(oldId, entityId)); // TODO
        }

        void clear () {
            logging::log(LEVEL_DEBUG, "Clearing entities!");
            numEntities = 0;
        }

        template <typename T, typename F, typename ...List>
        void applyFunc (F f, List... args) {
            static_assert(meta::is_callable<meta::add_pointer<T>, int, int, List...>(f), "Function has incorrect parameters!");
            f(std::get<meta::add_pointer<T>>(ptrTuple), numEntities, 0, args...);
        }

        template <typename T, typename A, typename F, typename ...List>
        void applyFunc (F f, List... args) {
            static_assert(meta::is_callable<meta::add_pointer<T>, int, int, A, List...>("Function has incorrect parameters!"));
            f(std::get<meta::add_pointer<T>>(ptrTuple), numEntities, 0, std::get<meta::add_pointer<A>>(ptrTuple), args...);
        }

        friend class view::ViewCore<Args...>;
    };


    template<typename T, typename = typename T::args>
    struct ComponentManagerWrap;
    template<typename T, typename... Args>
    struct ComponentManagerWrap <T, meta::type_list<Args...>> {
        using type = ComponentManagerImpl<Args...>;
    };
    template <typename T>
    using ComponentManager2 = typename ComponentManagerWrap<T>::type;


    using entity_list = meta::type_list_wrapper<game::AbstractEntity*, component::EntityMainComponent,
            graphics::FixedModel, physics::CollisionComponent, graphics::AbsolutePosition, RotationComponent, game::EntityType, std::shared_ptr<game::EntityController>>; // TODO: remove includes like AbstractEntity*

    using EntityComponentManager = ComponentManager2<entity_list>;

    extern template class ComponentManagerWrap<entity_list>;
}
#endif