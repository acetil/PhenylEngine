#pragma once

#include <cstddef>
#include <cstring>
#include <memory>
#include <utility>
#include <tuple>
#include <vector>

#include "util/meta.h"
#include "util/map.h"
#include "util/smart_help.h"
#include "util/optional.h"
#include "util/fixed_stack.h"

namespace component {

    namespace detail {
        template <typename T>
        void deleterFunc (unsigned char* tPtr) {
            ((T*)tPtr)->~T();
        }
    }

    namespace view {
        class EntityView;
        template <typename ...Args>
        class ConstrainedEntityView;
        template <typename ...Args>
        class ConstrainedView;

        namespace detail {
            class EntityViewIterator;
        }
    }

    class ComponentManagerNew;

    struct EntityId {
    private:
        unsigned int generation;
        unsigned int id;
    public:
        EntityId () = default;
        EntityId (unsigned int _generation, unsigned int _id) : generation(_generation), id(_id) {}

        [[nodiscard]] std::size_t value () const {
            return (static_cast<std::size_t>(generation) << 32) | id;
        }

        friend class ComponentManagerNew;
    };

    class ComponentManagerNew : public util::SmartHelper<ComponentManagerNew, true> {
    private:
        using DeleterFunc = void (*)(unsigned char*);
        std::size_t numEntities{};
        std::size_t maxNumEntities{};
        std::vector<std::tuple<std::unique_ptr<unsigned char[]>, std::size_t, DeleterFunc>> components;

        util::Map<std::size_t, std::size_t> compMap;

        std::unique_ptr<std::pair<unsigned int, std::size_t>[]> ids;

        util::FixedStack<unsigned int> availableIds;


        template <typename T>
        bool hasComp () {
            auto typeId = meta::type_index<T>::val();

            return compMap.contains(typeId);
        }

        template <typename T,  typename ...Args>
        auto hasAllComps () -> std::enable_if_t<sizeof...(Args) != 0, bool> {
            return hasComp<T>() && hasAllComps<Args...>();
        }

        template <typename T>
        bool hasAllComps () {
            return hasComp<T>();
        }

        template <typename T>
        void addComp () {
            auto typeId = meta::type_index<T>::val();

            auto pos = components.size();

            components.emplace_back(std::make_unique<unsigned char[]>(sizeof(T) * maxNumEntities), sizeof(T), detail::deleterFunc<T>);

            compMap[typeId] = pos;
        }

        template <typename T>
        T* getOrCreate () {
            auto typeId = meta::type_index<T>::val();
            if (!compMap.contains(typeId)) {
                addComp<T>();
            }
            return reinterpret_cast<T*>(std::get<0>(components[compMap[typeId]]).get());
        }

        util::Optional<std::size_t> getEntityPos (EntityId entityId) const {
#ifndef NDEBUG
            if (entityId.id >= maxNumEntities) {
                logging::log(LEVEL_ERROR, "Bad entity id {}: maxEntities = {}, id num = {}", entityId.value(), maxNumEntities, entityId.id);
                throw std::out_of_range("Bad entity id");
            }
#endif

            if (entityId.generation != ids[entityId.id].first) {
                // Gen 0 is reserved for non-existent
                logging::log(LEVEL_WARNING, "Bad entity id {}: id gen = {}, curr gen = {}", entityId.value(), entityId.generation, ids[entityId.id].first);
                return util::NullOpt;
            }

            return util::Optional<std::size_t>(ids[entityId.id].second);
        }

        void swapLastEntity (std::size_t entityPos, unsigned int entityId) {
            if (entityPos == numEntities - 1) {
                numEntities--;
                return;
            }

            for (auto& [x, size, deleter] : components) {
                deleter(x.get() + size * entityPos);
                std::memcpy(x.get() + size * entityPos, x.get() + size * (numEntities - 1), size);
            }
            getComponent<EntityId>().ifPresent([this, &entityPos](auto& ptr) {
                ids[ptr[entityPos].id].second = entityPos;
            });

            availableIds.push(std::move(entityId));

            numEntities--;
        }

        void intDestroyEntity (std::size_t pos) {
            auto entityId = getComponent<EntityId>().orElse(nullptr)[pos];
            swapLastEntity(pos, entityId.id);

            ids[entityId.id].first++;
            if (ids[entityId.id].first == 0) {
                ids[entityId.id].first = 1;
            }
        }

    public:
        using iterator = view::detail::EntityViewIterator;
        using const_iterator = view::detail::EntityViewIterator;

        explicit ComponentManagerNew (std::size_t maxEntities) : maxNumEntities{maxEntities}, ids{std::make_unique<std::pair<unsigned int, std::size_t>[]>(maxEntities)},
                                                                 availableIds{maxEntities} {
            for (std::size_t i = 0; i < maxNumEntities; i++) {
                ids[i] = {1, 0};
                availableIds.push(maxNumEntities - i - 1);
            }
        }

        ~ComponentManagerNew() {
            for (auto& [x, size, deleter] : components) {
                for (int i = 0; i < numEntities; i++) {
                    deleter(x.get() + size * i);
                }
            }
        }

        template <typename T>
        void addComponentType () {
            addComp<T>();
        }

        template <typename T>
        util::Optional<T*> getComponent () {
            auto typeId = meta::type_index<T>::val();
            if (compMap.contains(typeId)) {
                return util::Optional<T*>(reinterpret_cast<T*>(std::get<0>(components[compMap[typeId]]).get()));
            } else {
                return util::NullOpt;
            }
        }

        template <typename T>
        util::Optional<T&> getObjectData (EntityId entityId) {
            auto entityPos = getEntityPos(entityId);

            return getComponent<T>()
                .then([&entityPos](T* ptr){
                    return entityPos.then([&ptr](const std::size_t& pos) {return util::Optional<T&>(*(ptr + pos));});
                });
        }

        template <typename T>
        util::Optional<T*> getObjectDataPtr (EntityId entityId) {
            auto entityPos = getEntityPos(entityId);

            return getComponent<T>().then([&entityPos](T* ptr) {
                return entityPos.thenMap([&ptr](const std::size_t& pos){return ptr + pos;});
            });
        }

        template <typename T, typename ...Args>
        void addComponent (EntityId entityId, Args... args) {
            getEntityPos(entityId)
                .ifPresent([&args..., this](auto& pos) {
                    new(getOrCreate<T>() + pos) T(args...);
                });
        }

        EntityId createEntity () {
            auto idPos = availableIds.pop();

            auto entityPos = numEntities++;

            auto gen = ids[idPos].first;
            ids[idPos].second = entityPos;

            EntityId id{gen, idPos};

            addComponent<EntityId>(id, id);

            return id;
        }

        void removeEntity (EntityId entityId) {
            getEntityPos(entityId).ifPresent([this, &entityId](auto& pos) {
                swapLastEntity(pos, entityId.id);

                ids[entityId.id].first++;
                if (ids[entityId.id].first == 0) {
                    ids[entityId.id].first = 1;
                }
            });
        }

        std::size_t getNumObjects () {
            return numEntities;
        }

        void clear () {
            logging::log(LEVEL_DEBUG, "Clearing entities!");
            while (numEntities > 0) {
                intDestroyEntity(numEntities - 1);
            }
        }

        /*template <typename T, typename F, typename ...Args>
        void applyFunc (F f, Args... args) {
            static_assert(meta::is_callable<meta::add_pointer<T>, int, int, Args...>(f), "Function has incorrect parameters!");
            getComponent<T>().ifPresent([&f, &args..., this] (T* ptr) {
                f(ptr, numEntities, 0, args...);
            });
            //f(std::get<meta::add_pointer<T>>(ptrTuple), numEntities, 0, args...);
        }

        template <typename T, typename A, typename F, typename ...Args>
        void applyFunc (F f, Args... args) {
            static_assert(meta::is_callable<meta::add_pointer<T>, meta::add_pointer<A>, int, int, Args...>(f), "Function has incorrect parameters!");
            getComponent<T>().ifPresent([&f, &args..., this] (T* ptr) {
                getComponent<A>().ifPresent([&f, &args..., ptr, this] (A* ptr1) {
                    f(ptr, ptr1, numEntities, 0, args...);
                });
            });
        }*/

        util::Optional<std::size_t> tempGetPos (EntityId entityId) const {
            return getEntityPos(entityId);
        }

        iterator begin ();
        iterator end ();

        const_iterator cbegin ();
        const_iterator cend ();

        // TODO
        inline view::EntityView getEntityView (EntityId entityId);
        // TODO
        template <typename ...Args>
        util::Optional<view::ConstrainedEntityView<Args...>> getConstrainedEntityView (EntityId entityId);

        template <typename ...Args>
        util::Optional<view::ConstrainedView<Args...>> getConstrainedView ();

        friend view::detail::EntityViewIterator;
    };

}