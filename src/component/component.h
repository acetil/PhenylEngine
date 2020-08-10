#include <unordered_map>
#include <string>
#include <vector>
#include <type_traits>
#include "event/events/object_id_swap.h"

#include <cmath>
#include <graphics/graphics_new_include.h>
#include <physics/collision_component.h>
#include "event/event.h"
#include "main_component.h"

#ifndef MAX_COMPONENT_ENTITIES
#define MAX_COMPONENT_ENTITIES 256
#endif
#ifndef COMPONENT_H
#define COMPONENT_H
#if MAX_COMPONENT_ENTITIES <= 256
namespace component {
    typedef unsigned char id_type_t;
}
#else
namespace component {
    typedef unsigned short id_type_t;
}
#endif
#ifndef ENTITY_H
namespace game {
    class AbstractEntity;
}
#endif
namespace component {
    template <typename T>
    using add_pointer = T*;

    template <typename C, typename ...Args>
    struct get_first_impl {
        using type = C;
    };
    template <typename ...Args>
    using get_first = typename get_first_impl<Args...>::type;


    // The below can_call from https://stackoverflow.com/a/22882504
    struct can_call_test
    {
        template<typename F, typename... A>
        static decltype(std::declval<F>()(std::declval<A>()...), std::true_type())
        f(int);

        template<typename F, typename... A>
        static std::false_type
        f(...);
    };

    template<typename F, typename... A>
    using can_call = decltype(can_call_test::f<F, A...>(0));

    template<typename ...T, typename F>
    constexpr bool is_callable(F&&) { return can_call<F, T...>{}; }

    template <typename ...Args>
    class ComponentManagerImpl {
        using FirstType = get_first<Args...>;
    private:
        std::tuple<add_pointer<Args>...> ptrTuple;
        id_type_t numEntities;
        id_type_t maxNumEntities;
        event::EventBus* eventBus;
    public:
        //template <int N, typename T>
        //void initData () {

        //}
        template <int N = 0, std::enable_if_t<N < sizeof...(Args), int> = 0>
        void initData () {
            using T = typename std::remove_pointer<typename std::remove_reference<decltype(std::get<N>(ptrTuple))>::type>::type;
            std::get<N>(ptrTuple) = new T[maxNumEntities];
            initData<N + 1>();
        }
        template <int N, std::enable_if_t<N >= sizeof...(Args), int> = 0>
        void initData () {

        }
        template <int N = 0, typename T, typename ...List>
        void destroyData () {
            if constexpr (N == sizeof...(Args)) {
                return;
            }
            delete[] std::get<N>(ptrTuple);
            destroyData<N + 1, List...>();
        }

        explicit ComponentManagerImpl(id_type_t maxEntities, event::EventBus* bus) {
            maxNumEntities = maxEntities;
            numEntities = 0;
            eventBus = bus;
            initData();
        }
        ~ComponentManagerImpl () {
            destroyData();
        }

        template <typename T>
        T* getComponent () {
            return std::get<add_pointer<T>>(ptrTuple);
        }
        template <int N>
        decltype(std::get<N>(ptrTuple)) getComponent () {
            return std::get<N>(ptrTuple);
        }
        template <typename T>
        T& getObjectData (int entityId) {
            T* temp = std::get<add_pointer<T>>(ptrTuple) + entityId;
            //static_assert(std::is_same<decltype(temp2),T&>::value, "Typecast failed!");
            return reinterpret_cast<T&>(*temp);
        }
        template <typename T>
        T* getObjectDataPtr (int entityId) {
            return std::get<add_pointer<T>>(ptrTuple) + entityId;
        }
        id_type_t getNumObjects () {
            return numEntities;
        }
        id_type_t addObject (FirstType obj) {
            logging::logf(LEVEL_DEBUG, "New object! Num objects: %d", numEntities + 1);
            std::get<0>(ptrTuple)[numEntities] = obj;
            return numEntities++;
        }
        template <int N = 0, typename T, typename ...List>
        void swapObjects (id_type_t oldId, id_type_t newId) {
            std::get<N>(ptrTuple)[newId] = std::get<N>(ptrTuple)[oldId];
            swapObjects<N + 1, List...>();
        }
        void removeObject (id_type_t entityId) {
            if (entityId >= numEntities) {
                logging::logf(LEVEL_ERROR, "Attempted to remove object id %d, only %d object components!", entityId, numEntities);
                return;
            }
            id_type_t oldId = --numEntities;
            swapObjects<Args...>(oldId, entityId);
            eventBus->raiseEvent(new event::ObjectIdSwapEvent<FirstType>(oldId, entityId));
        }

        template <typename T, typename F, typename ...List>
        void applyFunc (F f, List... args) {
            static_assert(is_callable<add_pointer<T>, int, int, List...>(f), "Function has incorrect parameters!");
            f(std::get<add_pointer<T>>(ptrTuple), numEntities, 0, args...);
        }

        template <typename T, typename A, typename F, typename ...List>
        void applyFunc (F f, List... args) {
            static_assert(is_callable<add_pointer<T>, int, int, A, List...>("Function has incorrect parameters!"));
            f(std::get<add_pointer<T>>(ptrTuple), numEntities, 0, std::get<add_pointer<A>>(ptrTuple), args...);
        }

    };
    template<typename ...T>
    struct type_list {};

    template <typename ...T>
    struct type_list_wrapper {
        using args = type_list<T...>;
    };


    template<typename T, typename = typename T::args>
    struct ComponentManagerWrap;
    template<typename T, typename... Args>
    struct ComponentManagerWrap <T, type_list<Args...>> {
        using type = ComponentManagerImpl<Args...>;
    };
    template <typename T>
    using ComponentManager2 = typename ComponentManagerWrap<T>::type;


    using entity_list = type_list_wrapper<game::AbstractEntity*, component::EntityMainComponent,
            graphics::FixedModel, physics::CollisionComponent, graphics::AbsolutePosition>; // TODO: remove includes like AbstractEntity*

    using EntityComponentManager = ComponentManager2<entity_list>;

    extern template class ComponentManagerWrap<entity_list>;
}
#endif