#pragma once

#ifndef PHENYL_MAX_COMPONENTS
#define PHENYL_MAX_COMPONENTS 64
#endif

//#include <string>
#include <utility>
#include <vector>
#include <memory>

//#include <cmath>
#include "component/forward.h"
#include "util/meta.h"
#include "util/smart_help.h"
#include "util/bitfield.h"
#include "util/fixed_stack.h"
#include "util/optional.h"
#include "util/map.h"

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

    namespace detail {
        template <std::size_t MaxComponents>
        class EntityViewIterator;
    }

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
        using iterator = detail::EntityViewIterator<MaxComponents>;
        using const_iterator = detail::EntityViewIterator<MaxComponents>;

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

        component::ComponentView<MaxComponents> createEntity () {
            auto idPos = availableIds.pop();

            auto entityPos = numEntities++;

            auto gen = ids[idPos].first;
            ids[idPos].second = entityPos;

            EntityId id{gen, idPos};

            addComponent<EntityId>(id, id);

            return getEntityView(id);
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

        util::Optional<std::size_t> tempGetPos (EntityId entityId) const {
            return getEntityPos(entityId);
        }

        iterator begin ();
        iterator end ();

        const_iterator cbegin ();
        const_iterator cend ();

        // TODO
        inline ComponentView<MaxComponents> getEntityView (EntityId entityId);
        // TODO
        template <typename ...Args>
        util::Optional<ConstrainedEntityView<MaxComponents, Args...>> getConstrainedEntityView (EntityId entityId);

        template <typename ...Args>
        ConstrainedView<MaxComponents, Args...> getConstrainedView ();

        friend detail::EntityViewIterator<MaxComponents>;
        friend ComponentView<MaxComponents>;
    };

    extern template class ComponentManagerNew<PHENYL_MAX_COMPONENTS>;


    template <std::size_t MaxComponents>
    class ComponentView {
    private:
        component::EntityId entityId;
        typename component::ComponentManagerNew<MaxComponents>::SharedPtr compManager;

        template <typename T, typename ...Ts>
        bool allValid () {
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
        ComponentView() : entityId{0, 0}, compManager(nullptr) {}
    public:
        ComponentView( component::EntityId _entityId, typename component::ComponentManagerNew<MaxComponents>::SharedPtr  _compManager) : entityId{_entityId}, compManager{std::move(_compManager)} {}

        component::EntityId getId () {
            return entityId;
        }

        template <typename T>
        util::Optional<T&> getComponent () {
            return compManager->template getObjectData<T>(entityId);
        }

        template <typename T, typename ...Args>
        void addComponent (Args&&... args) {
            compManager->template addComponent<T>(entityId, std::forward<Args>(args)...);
        }

        template <typename T>
        void removeComponent () {
            compManager->template removeComponent<T>(entityId);
        }

        /*ComponentView<MaxComponents> withId (component::EntityId newId) {
            return {newId, compManager};
        }*/

        template <typename ...Ts, typename F>
        void applyFunc (F f) {
            if (allValid<Ts...>()) {
                getAllComps<Ts...>().ifPresent([&f](std::tuple<Ts&...>& tup) {
                    f(std::get<Ts&>(tup)...);
                });
            }
        }

        template<class T>
        bool hasComponent () {
            return compManager->template entityHasComp<T>(entityId);
        }

        friend detail::EntityViewIterator<MaxComponents>;
    };

    extern template class ComponentView<PHENYL_MAX_COMPONENTS>;

    namespace detail {
        template <std::size_t MaxComponents>
        class EntityViewIterator {
        private:
            typename ComponentManagerNew<MaxComponents>::SharedPtr compManager{};
            std::size_t pos{};

        public:
            EntityViewIterator () {}
            EntityViewIterator (typename ComponentManagerNew<MaxComponents>::SharedPtr _compManager, std::size_t startPos) : compManager{std::move(_compManager)} , pos{startPos} {}
            using iterator_category = std::random_access_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = ComponentView<MaxComponents>;
            using reference = ComponentView<MaxComponents>&;
            value_type operator* () const {
                return compManager->getEntityView(compManager->template getComponent<component::EntityId>().orThrow()[pos]);
            }

            value_type operator[] (std::ptrdiff_t shift) const {
                return *(*this + shift);
            }

            EntityViewIterator<MaxComponents>& operator++ () {
                pos++;
                return *this;
            }

            EntityViewIterator<MaxComponents> operator++ (int) {
                EntityViewIterator<MaxComponents> other = *this;

                pos++;

                return other;
            }

            bool operator== (const EntityViewIterator<MaxComponents>& other) const {
                return pos == other.pos;
            }

            EntityViewIterator<MaxComponents>& operator-- () {
                pos--;
                return *this;
            }

            EntityViewIterator<MaxComponents> operator-- (int) {
                EntityViewIterator<MaxComponents> other = *this;

                pos--;

                return other;
            }

            template <std::size_t N>
            friend EntityViewIterator<N> operator+ (EntityViewIterator<N> lhs, const std::ptrdiff_t rhs);
            template <std::size_t N>
            friend EntityViewIterator<N> operator+ (const std::ptrdiff_t rhs, EntityViewIterator<N> lhs);
            template <std::size_t N>
            friend EntityViewIterator<N> operator- (EntityViewIterator<N> lhs, const std::ptrdiff_t rhs);

            EntityViewIterator<MaxComponents>& operator+= (std::ptrdiff_t amount) {
                pos += amount;
                return *this;
            }

            EntityViewIterator<MaxComponents>& operator-= (std::ptrdiff_t amount) {
                pos -= amount;
                return *this;
            }

            bool operator< (const EntityViewIterator<MaxComponents>& other) const {
                return pos < other.pos;
            }

            bool operator<= (const EntityViewIterator<MaxComponents>& other) const {
                return !(*this > other);
            }

            bool operator> (const EntityViewIterator<MaxComponents>& other) const {
                return other < *this;
            }

            bool operator>= (const EntityViewIterator<MaxComponents>& other) const {
                return !(*this < other);
            }

            std::ptrdiff_t operator- (const EntityViewIterator<MaxComponents>& other) const {
                return (std::ptrdiff_t)pos - (std::ptrdiff_t)other.pos;
            }
        };

        template <std::size_t MaxComponents>
        inline EntityViewIterator<MaxComponents> operator+ (EntityViewIterator<MaxComponents> lhs, const std::ptrdiff_t rhs) {
            lhs += rhs;

            return lhs;
        }

        template <std::size_t MaxComponents>
        inline EntityViewIterator<MaxComponents> operator+ (const std::ptrdiff_t rhs, EntityViewIterator<MaxComponents> lhs) {
            return std::move(lhs) + rhs;
        }

        template <std::size_t MaxComponents>
        inline EntityViewIterator<MaxComponents> operator- (EntityViewIterator<MaxComponents> lhs, const std::ptrdiff_t rhs) {
            lhs -= rhs;

            return lhs;
        }
    }

    namespace detail {
        template<std::size_t MaxComponents, typename ...Args>
        class ConstrainedViewIterator;
    }

    template<std::size_t MaxComponents, typename ...Args>
    class ConstrainedView;

    template<std::size_t MaxComponents, typename ...Args>
    class ConstrainedEntityView {
    private:
        std::tuple<Args& ...> comps;
        component::EntityId entityId;

        explicit ConstrainedEntityView (component::EntityId _entityId, Args& ... args) : entityId{_entityId},
                                                                                         comps{args...} {}

    public:
        ConstrainedEntityView (ConstrainedEntityView&) = default;

        ConstrainedEntityView (ConstrainedEntityView&&) noexcept = default;

        template<typename T>
        T& get () const {
            return std::get<T&>(comps);
        }

        [[nodiscard]] component::EntityId getId () const {
            return entityId;
        }

        template<typename ...Args2>
        ConstrainedEntityView<MaxComponents, Args2...> constrain () {
            static_assert(meta::is_all_in<meta::type_list_wrapper<Args...>, Args2...>,
                          "All requested types must be accessible!");
            /*if constexpr (meta::is_all_in<meta::type_list_wrapper<Args...>, Args2...>) {
                return util::Optional<ConstrainedEntityView<Args2...>>(ConstrainedEntityView(entityId, std::get<Args2...>(comps)));
            } else {
                return util::NullOpt;
            }*/
            return {entityId, std::get<Args2...>(comps)};
        }

        friend ComponentManagerNew<MaxComponents>;
        friend ConstrainedView<MaxComponents, Args...>;
        friend detail::ConstrainedViewIterator<MaxComponents, Args...>;
    };

    template<std::size_t MaxComponents, typename ...Args>
    class ConstrainedView {
    private:
        std::tuple<util::Bitfield<MaxComponents>*, component::EntityId*, Args* ...> comps{};
        typename component::ComponentManagerNew<MaxComponents>::SharedPtr componentManager{nullptr};
        util::Bitfield<MaxComponents> mask{};

        ConstrainedView () = default;

        std::size_t numObjects () {
            return componentManager->getNumObjects();
        }

    public:
        using iterator = detail::ConstrainedViewIterator<MaxComponents, Args...>;

        ConstrainedView (typename component::ComponentManagerNew<MaxComponents>::SharedPtr _compManager,
                         util::Bitfield<MaxComponents>* bitfields, component::EntityId* ids, Args* ... compPtrs,
                         util::Bitfield<MaxComponents> mask = {}) : componentManager{std::move(_compManager)},
                                                                    comps{bitfields, ids, compPtrs...},
                                                                    mask{mask} {}

        util::Optional<ConstrainedEntityView<MaxComponents, Args...>>
        getEntityView (component::EntityId entityId) const {
            if (!componentManager) {
                return util::NullOpt;
            }

            return componentManager->tempGetPos(entityId).then(
                    [this] (const size_t& pos) -> util::Optional<ConstrainedEntityView<MaxComponents, Args...>> {
                        if ((std::get<util::Bitfield<MaxComponents>*>(comps)[pos] & mask) == mask) {
                            return {ConstrainedEntityView<MaxComponents, Args...>{std::get<EntityId*>(comps)[pos],
                                                                                  std::get<Args*>(comps)[pos]...}};
                        } else {
                            return util::NullOpt;
                        }
                    });
        }

        template<typename ...Args2>
        ConstrainedView<MaxComponents, Args2...> constrain () const {
            static_assert(meta::is_all_in<meta::type_list_wrapper<Args...>, Args2...>,
                          "All requested types must be accessible!");

            return ConstrainedView<MaxComponents, Args2...>(componentManager,
                                                            std::get<util::Bitfield<MaxComponents>*>(comps),
                                                            std::get<EntityId*>(comps), std::get<Args2*>(comps)...,
                                                            mask);
        }

        inline iterator begin ();

        inline iterator end ();

        friend detail::ConstrainedViewIterator<MaxComponents, Args...>;
        friend ComponentManagerNew<MaxComponents>;
    };

    namespace detail {
        template<std::size_t MaxComponents, typename ...Args>
        class ConstrainedViewIterator {
        private:
            ConstrainedView<MaxComponents, Args...> constrainedView;
            std::size_t pos{};

            bool isValidObject (std::size_t checkPos) {
                auto val = std::get<util::Bitfield<MaxComponents>*>(constrainedView.comps)[checkPos];
                return (val & constrainedView.mask) == constrainedView.mask;
            }

            void findFirst () {
                while (pos < constrainedView.numObjects() && !isValidObject(pos)) {
                    pos++;
                }
            }

            void findNext () {
                pos++;

                while (pos < constrainedView.numObjects() && !isValidObject(pos)) {
                    pos++;
                }
            }

            void findPrev () {
                pos--;
                while (pos >= 0 && !isValidObject(pos)) {
                    pos--;
                }
            }

        public:
            ConstrainedViewIterator () = default;

            ConstrainedViewIterator (ConstrainedView<MaxComponents, Args...> _constrainedView, std::size_t startPos)
                    : constrainedView{
                    _constrainedView}, pos{startPos} {
                findFirst();
            }

            using iterator_category = std::bidirectional_iterator_tag;
            using value_type = ConstrainedEntityView<MaxComponents, Args...>;
            using difference_type = std::ptrdiff_t;

            value_type operator* () const {
                return ConstrainedEntityView<MaxComponents, Args...>(
                        std::get<component::EntityId*>(constrainedView.comps)[pos],
                        std::get<Args*>(constrainedView.comps)[pos]...);
            }

            ConstrainedViewIterator<MaxComponents, Args...>& operator++ () {
                findNext();
                return *this;
            }

            ConstrainedViewIterator<MaxComponents, Args...> operator++ (int) {
                ConstrainedViewIterator other = *this;
                findNext();
                return other;
            }

            ConstrainedViewIterator<MaxComponents, Args...>& operator-- () {
                findPrev();
                return *this;
            }

            ConstrainedViewIterator<MaxComponents, Args...> operator-- (int) {
                ConstrainedViewIterator other = *this;
                findPrev();
                return other;
            }

            bool operator== (const ConstrainedViewIterator<MaxComponents, Args...>& other) const {
                return pos == other.pos ||
                       (!constrainedView.componentManager && !other.constrainedView.componentManager);
            }
        };
    }

    template<std::size_t N, typename ...Args>
    inline typename ConstrainedView<N, Args...>::iterator ConstrainedView<N, Args...>::begin () {
        return {*this, 0};
    }

    template<std::size_t N, typename ...Args>
    inline typename ConstrainedView<N, Args...>::iterator ConstrainedView<N, Args...>::end () {
        if (!componentManager) {
            return begin();
        }
        return {*this, componentManager->getNumObjects()};
    }
}

namespace component {
    template <std::size_t N>
    inline component::ComponentView<N> ComponentManagerNew<N>::getEntityView (EntityId entityId) {
        return {entityId, this->shared_from_this()};
    }

    template <std::size_t N>
    template <typename ...Args>
    util::Optional<ConstrainedEntityView<N, Args...>> ComponentManagerNew<N>::getConstrainedEntityView (EntityId entityId) {
        if (entityHasAllComps<Args...>(entityId)) {
            return util::Optional<ConstrainedEntityView<N, Args...>>(ConstrainedEntityView<N, Args...>{entityId, getObjectData<Args>(entityId).getUnsafe()...});
        } else {
            return util::NullOpt;
        }
    }

    template <std::size_t N>
    template <typename ...Args>
    ConstrainedView<N, Args...> ComponentManagerNew<N>::getConstrainedView () {
        if (hasAllComps<Args...>()) {
            return {this->shared_from_this(), this->entityComponentBitmaps.get(), getComponent<EntityId>().getUnsafe(), getComponent<Args>().getUnsafe()...,
                    makeMask<Args...>()};
        } else {
            logging::log(LEVEL_ERROR, "Failed to find all component types!");
            return {};
        }
    }

    template <std::size_t N>
    inline typename ComponentManagerNew<N>::iterator ComponentManagerNew<N>::begin () {
        return {this->shared_from_this(), 0};
    }

    template <std::size_t N>
    inline typename ComponentManagerNew<N>::iterator ComponentManagerNew<N>::end () {
        return {this->shared_from_this(), numEntities};
    }

    template <std::size_t N>
    inline typename ComponentManagerNew<N>::const_iterator ComponentManagerNew<N>::cbegin () {
        return {this->shared_from_this(), 0};
    }

    template <std::size_t N>
    inline typename ComponentManagerNew<N>::const_iterator ComponentManagerNew<N>::cend () {
        return {this->shared_from_this(), numEntities};
    }
}