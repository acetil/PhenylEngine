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
#include "util/bitfield.h"

namespace component {
    namespace detail {
        template <typename T>
        void deleterFunc (unsigned char* tPtr) {
            ((T*)tPtr)->~T();
        }

        struct ComponentType {
        private:
            using DeleterFunc = void (*)(unsigned char*);
        public:
            std::unique_ptr<unsigned char[]> components;
            std::size_t componentSize;
            DeleterFunc deleter;
        };
    }

    namespace view {
        template <std::size_t MaxComponents>
        class ComponentView;
        template <std::size_t MaxComponents, typename ...Args>
        class ConstrainedEntityView;
        template <std::size_t MaxComponents, typename ...Args>
        class ConstrainedView;

        namespace detail {
            template <std::size_t MaxComponents>
            class EntityViewIterator;
        }
    }

    template <std::size_t MaxComponents>
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

        template <std::size_t N>
        friend class ComponentManagerNew;
    };

    template <std::size_t MaxComponents>
    class ComponentManagerNew : public util::SmartHelper<ComponentManagerNew<MaxComponents>, true> {
    private:
        using DeleterFunc = void (*)(unsigned char*);
        std::size_t numEntities{};
        std::size_t maxNumEntities{};
        util::Bitfield<MaxComponents> componentBitmap;
        std::unique_ptr<util::Bitfield<MaxComponents>[]> entityComponentBitmaps;
        std::vector<detail::ComponentType> components;

        util::Map<std::size_t, std::size_t> compMap;

        std::unique_ptr<std::pair<unsigned int, std::size_t>[]> ids;

        util::FixedStack<unsigned int> availableIds;


        template <typename T>
        bool hasComp () {
            auto typeId = meta::type_index<T>();

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
        bool entityHasComp (EntityId id) {
            auto typeId = meta::type_index<T>();
            return compMap.contains(typeId) && getEntityPos(id).thenMap([this, typeId](const std::size_t& pos) -> bool {
                return entityComponentBitmaps[pos].hasBit(compMap.at(typeId));
            }).orElse(false);
            //return compMap.contains(typeId) && entityComponentBitmaps[];
        }

        template <typename T,  typename ...Args>
        auto entityHasAllComps (EntityId id) -> std::enable_if_t<sizeof...(Args) != 0, bool> {
            return entityHasComp<T>(id) && entityHasAllComps<Args...>(id);
        }

        template <typename T>
        bool entityHasAllComps (EntityId id) {
            return entityHasComp<T>(id);
        }

        template <typename T>
        void addComp () {
            assert(components.size() < MaxComponents);
            auto typeId = meta::type_index<T>();

            auto pos = components.size();

            components.emplace_back(detail::ComponentType{std::make_unique<unsigned char[]>(sizeof(T) * maxNumEntities), sizeof(T), detail::deleterFunc<T>});

            compMap[typeId] = pos;
            componentBitmap.putBit(pos);
        }

        template <typename T>
        T* getOrCreate () {
            auto typeId = meta::type_index<T>();
            if (!compMap.contains(typeId)) {
                addComp<T>();
            }
            return reinterpret_cast<T*>(components[compMap[typeId]].components.get());
        }

        [[nodiscard]] util::Optional<std::size_t> getEntityPos (EntityId entityId) const {
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

            return {ids[entityId.id].second};
        }

        void swapLastEntity (std::size_t entityPos, unsigned int entityId) {
            if (entityPos == numEntities - 1) {
                numEntities--;
                return;
            }
            std::size_t index = 0;
            for (auto& [x, size, deleter] : components) {
                if (entityComponentBitmaps[entityPos].hasBit(index)) {
                    deleter(x.get() + size * entityPos);
                }

                if (entityComponentBitmaps[numEntities - 1].hasBit(index)) {
                    // TODO
                    std::memcpy(x.get() + size * entityPos, x.get() + size * (numEntities - 1), size);
                }
                index++;
            }
            getComponent<EntityId>().ifPresent([this, &entityPos](auto& ptr) {
                ids[ptr[entityPos].id].second = entityPos;
            });

            entityComponentBitmaps[entityPos] = entityComponentBitmaps[numEntities - 1];

            // Move necessary because cpp is dumb
            // TODO
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

        template <typename T, typename ...Args>
        std::enable_if_t<sizeof...(Args) != 0, util::Bitfield<MaxComponents>> makeMask () {
            auto mask1 = makeMask<T>();
            auto mask2 = makeMask<Args...>();
            return mask1 | mask2;
        }

        template <typename T>
        util::Bitfield<MaxComponents> makeMask () {
            auto typeId = meta::type_index<T>();
            if (!compMap.contains(typeId)) {
                logging::log(LEVEL_DEBUG, "Not in comp map!");
                return {};
            }
            util::Bitfield<MaxComponents> bitfield;
            auto compId = compMap.at(typeId);
            bitfield.putBit(compId);
            return bitfield;
        }

    public:
        using iterator = view::detail::EntityViewIterator<MaxComponents>;
        using const_iterator = view::detail::EntityViewIterator<MaxComponents>;

        explicit ComponentManagerNew (std::size_t maxEntities) : maxNumEntities{maxEntities}, ids{std::make_unique<std::pair<unsigned int, std::size_t>[]>(maxEntities)},
                                                                 availableIds{maxEntities}, entityComponentBitmaps{std::make_unique<util::Bitfield<MaxComponents>[]>(maxEntities)} {
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
            auto typeId = meta::type_index<T>();
            if (compMap.contains(typeId)) {
                return util::Optional<T*>(reinterpret_cast<T*>(components[compMap[typeId]].components.get()));
            } else {
                return util::NullOpt;
            }
        }

        template <typename T>
        util::Optional<T&> getObjectData (EntityId entityId) {
            auto entityPos = getEntityPos(entityId);

            return getComponent<T>()
                .then([&entityPos, this](T* ptr) {
                    return entityPos.then([&ptr, this](const std::size_t& pos) -> util::Optional<T&> {
                        auto typeId = meta::type_index<T>();
                        auto compIndex = compMap.at(typeId);
                        if (entityComponentBitmaps[pos].hasBit(compIndex)) {
                            return util::Optional<T&>(*(ptr + pos));
                        } else {
                            return util::NullOpt;
                        }
                    });
                });
        }

        template <typename T>
        util::Optional<T*> getObjectDataPtr (EntityId entityId) {
            auto entityPos = getEntityPos(entityId);

            return getComponent<T>().then([&entityPos, this](T* ptr) {
                return entityPos.then([&ptr, this](const std::size_t& pos) -> util::Optional<T*> {
                    auto typeId = meta::type_index<T>();
                    auto compIndex = compMap.at(typeId);

                    if (entityComponentBitmaps[pos].hasBit(compIndex)) {
                        return util::Optional<T*>{ptr + pos};
                    } else {
                        return util::NullOpt;
                    }
                });
            });
        }

        template <typename T, typename ...Args>
        void addComponent (EntityId entityId, Args... args) {
            getEntityPos(entityId)
                .ifPresent([&args..., this](auto& pos) {
                    new(getOrCreate<T>() + pos) T(args...);

                    auto typeId = meta::type_index<T>();
                    auto compIndex = compMap.at(typeId);

                    entityComponentBitmaps[pos].putBit(compIndex);
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

        template <typename T>
        void removeComponent (EntityId entityId) {
            getEntityPos(entityId).ifPresent([this](const std::size_t& pos) {
                auto typeId = meta::type_index<T>();
                if (compMap.contains(typeId)) {
                    auto compIndex = compMap.at(typeId);
                    if (entityComponentBitmaps[pos].hasBit(compIndex)) {
                        auto& component = components[compIndex];
                        auto ptr = (T*) component.components.get();
                        ptr->~T();

                        entityComponentBitmaps[pos].maskBit(compIndex);
                    }
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
        inline view::ComponentView<MaxComponents> getEntityView (EntityId entityId);
        // TODO
        template <typename ...Args>
        util::Optional<view::ConstrainedEntityView<MaxComponents, Args...>> getConstrainedEntityView (EntityId entityId);

        template <typename ...Args>
        view::ConstrainedView<MaxComponents, Args...> getConstrainedView ();

        friend view::detail::EntityViewIterator<MaxComponents>;
        friend view::ComponentView<MaxComponents>;
    };

}