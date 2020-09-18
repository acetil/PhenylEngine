#include "graphics/maths_headers.h"
#include "component/component.h"
#include "util/meta.h"
#include "component/rotation_update.h"
#include <stddef.h>
#include <tuple>
#include <utility>
#include <graphics/graphics_new_include.h>

#ifndef VIEW_H
#define VIEW_H

// TODO: clean up

#ifndef ENTITY_H
namespace game {
    class AbstractEntity;
}
#endif

namespace view {

    template <typename T, typename V>
    class ViewPropertyCustom {
    protected:
        T* ptr;
        V& getUnderlying () {
            return static_cast<V&>(*this);
        }
    public:
        explicit ViewPropertyCustom(void* _ptr) : ptr{(T*)_ptr} {};
        template<typename U, typename W>
        auto operator+= (const ViewPropertyCustom<U, W>& val) -> decltype(*ptr += val()) {
            *ptr += val();
            getUnderlying().fieldChangeCallback();
            return *ptr;
        }
        template<typename U>
        auto operator+= (const U& val) -> decltype(*ptr += val) {
            *ptr += val;
            getUnderlying().fieldChangeCallback();
            return *ptr;
        }
        template<typename U>
        auto operator-= (const U& val) -> decltype(*ptr -= val) {
            *ptr -= val;
            getUnderlying().fieldChangeCallback();
            return *ptr;
        }
        template<typename U, typename W>
        auto operator-= (const ViewPropertyCustom<U, W>& val) -> decltype(*ptr -= val()) {
            *ptr -= val();
            getUnderlying().fieldChangeCallback();
            return *ptr;
        }
        template<typename U>
        auto operator*= (const U& val) -> decltype(*ptr *= val) {
            *ptr *= val;
            getUnderlying().fieldChangeCallback();
            return *ptr;
        }
        template<typename U, typename W>
        auto operator*= (const ViewPropertyCustom<U, W>& val) -> decltype(*ptr *= val()) {
            *ptr *= val();
            getUnderlying().fieldChangeCallback();
            return getUnderlying();
        }
        template<typename U>
        auto operator/= (const U& val) -> decltype(*ptr /= val) {
            *ptr /= val;
            getUnderlying().fieldChangeCallback();
            return *ptr;
        }
        template<typename U, typename W>
        auto operator/= (const ViewPropertyCustom<U, W>& val) -> decltype(*ptr /= val()) {
            *ptr /= val();
            getUnderlying().fieldChangeCallback();
            return *ptr;
        }
        auto operator() () -> decltype(*ptr)& {
            return reinterpret_cast<T&>(*ptr);
        }
        operator T() {
            return *ptr;
        }
        friend V;
    };
    template<typename T>
    class ViewProperty : public ViewPropertyCustom<T, ViewProperty<T>>{

    public:
        explicit ViewProperty(void* _ptr) : ViewPropertyCustom<T, ViewProperty<T>>(_ptr) {};
        explicit ViewProperty(T* _ptr) : ViewPropertyCustom<T, ViewProperty<T>>(_ptr) {};
        void fieldChangeCallback () {

        }
        template <typename U>
        ViewProperty<T>& operator= (const U& val){
            *(this->ptr) = val;
            fieldChangeCallback();
            return *this;
        }
    };
    template <typename T>
    using add_pointer = T*;

    template <typename ...Args>
    class ViewBaseImpl;

    template <typename ...Args>
    class ViewCore {
    private:
        const std::tuple<add_pointer<Args>...> pointerTuple;
        template <typename T>
        T* getPointer () {
            return std::get<add_pointer<T>>(pointerTuple);
        }
        component::EntityComponentManager* manager;
    public:
        /*explicit ViewCore(const component::ComponentManagerImpl<Args...>& compManager) {
            pointerTuple = std::move(compManager.ptrTuple);
        }*/
        explicit ViewCore(component::ComponentManagerImpl<Args...>* compManager) :
            pointerTuple{compManager->ptrTuple}, manager(compManager) {};
        //explicit ViewCore(std::tuple<add_pointer<Args>...> tup) : pointerTuple{tup} {};
        friend class ViewBaseImpl<Args...>;
        friend class ViewPropertyRotation;
    };
    template <typename ...Args>
    class ViewBaseImpl {
    protected:
        ViewCore<Args...> viewCore;
        template <typename T>
        T* getPointer () {
            return viewCore.template getPointer<T>();
        }
        explicit ViewBaseImpl(ViewCore<Args...> core) : viewCore{std::move(core)} {}
    };

/*#define VIEW_GET_PROPERTY_PTR (MANAGER_PTR, COMP_ID, STRUCT_TYPE, STRUCT_MEMBER, ID) \
    (((std::byte*)MANAGER_PTR.getObjectDataPtr<add_pointer<STRUCT_TYPE>>(COMP_ID, ID)) + offsetof(STRUCT_TYPE, STRUCT_MEMBER))*/
#define VIEW_GET_PROPERTY_PTR(STRUCT_TYPE, MEMBER_NAME, ENTITY_ID) \
    (&(getPointer<STRUCT_TYPE>()[ENTITY_ID].MEMBER_NAME))

    template <typename T, typename = typename T::args>
    struct ViewBaseWrap;

    template <typename T, typename ...Args>
    struct ViewBaseWrap <T, meta::type_list<Args...>> {
        using type = ViewBaseImpl<Args...>;
    };

    template <typename T, typename = typename T::args>
    struct ViewCoreListImpl;

    template <typename T, typename ...Args>
    struct ViewCoreListImpl <T, meta::type_list<Args...>> {
        using type = ViewCore<Args...>;
    };

    template <typename T>
    using ViewCoreList = typename ViewCoreListImpl<T>::type;

    template <typename T>
    using ViewBase = typename ViewBaseWrap<T>::type;

    class ViewPropertyRotation : public ViewPropertyCustom<float, ViewPropertyRotation> {
    private:
        float newVal;
        component::RotationComponent* compPtr;
        component::EntityComponentManager* manager;
        event::EventBus* eventBus;
        int entityId;
    public:
        explicit ViewPropertyRotation (ViewCoreList<component::entity_list> core,
                                       event::EventBus* bus, int _entityId) :
                ViewPropertyCustom<float, ViewPropertyRotation>(&newVal), compPtr(core.getPointer<component::RotationComponent>() + _entityId),
                newVal((core.getPointer<component::RotationComponent>() + _entityId)->rotation),
                manager(core.manager), eventBus(bus), entityId(_entityId) {};
        void fieldChangeCallback () {
            component::rotateEntity(entityId, newVal, manager, eventBus);
        }
        template<typename U>
        ViewPropertyRotation& operator= (const U& val) {
            newVal = val;
            fieldChangeCallback();
            return *this;
        }
    };

    class EntityView : public ViewBase<component::entity_list> {
    public:
        const int entityId;
        event::EventBus* eventBus;
        ViewProperty<game::AbstractEntity*> entity;

        ViewProperty<glm::vec2> position;
        ViewProperty<glm::vec2> velocity;
        ViewProperty<glm::vec2> acceleration;

        ViewProperty<float> constantFriction;
        ViewProperty<float> linearFriction;

        ViewProperty<graphics::FixedModel> model; // TODO: fix

        ViewPropertyRotation rotation;

        ViewProperty<unsigned int> collisionLayers;
        //ViewProperty<unsigned int> collisionMasks;
        ViewProperty<unsigned int> resolveLayers;
        ViewProperty<unsigned int> eventLayers;

        // TODO: add scaling
        EntityView (ViewCoreList<component::entity_list> core, int id, event::EventBus* bus) : ViewBase<component::entity_list>(core),
            entityId(id),
            eventBus(bus),
            rotation(core, bus, id),
            entity(getPointer<game::AbstractEntity*>() +  id),
            position(ViewProperty(VIEW_GET_PROPERTY_PTR(component::EntityMainComponent, pos, id))),
            velocity(ViewProperty(VIEW_GET_PROPERTY_PTR(component::EntityMainComponent, vel, id))),
            acceleration(ViewProperty(VIEW_GET_PROPERTY_PTR(component::EntityMainComponent, acc, id))),

            constantFriction(ViewProperty(VIEW_GET_PROPERTY_PTR(component::EntityMainComponent, constFriction, id))),
            linearFriction(ViewProperty(VIEW_GET_PROPERTY_PTR(component::EntityMainComponent, linFriction, id))),

            model(ViewProperty(getPointer<graphics::FixedModel>() + id)),

            collisionLayers(ViewProperty(VIEW_GET_PROPERTY_PTR(physics::CollisionComponent, layers, id))),
            resolveLayers(ViewProperty(VIEW_GET_PROPERTY_PTR(physics::CollisionComponent, resolveLayers, id))),
            eventLayers(ViewProperty(VIEW_GET_PROPERTY_PTR(physics::CollisionComponent, layers, id))) {};

        EntityView withId (int id) {
            return EntityView(viewCore, id, eventBus);
        }
    };
}
#endif //VIEW_H
