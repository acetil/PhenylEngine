#pragma once

#include "graphics/maths_headers.h"
#include "component/component.h"
#include "util/meta.h"
#include "component/rotation_update.h"
#include <stddef.h>
#include <tuple>
#include <utility>
#include "graphics/graphics_new_include.h"

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
        ViewProperty() : ViewPropertyCustom<T, ViewProperty<T>>(nullptr) {};
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

    class ViewBaseImpl;

    class ViewCore {
    private:
        //const std::tuple<add_pointer<Args>...> pointerTuple{};
        template <typename T>
        T* getPointer () {
            //return std::get<add_pointer<T>>(pointerTuple);
            return manager->getComponent<T>().orElse(nullptr);
        }

        std::size_t getPos (component::EntityId entityId) {
            return manager->tempGetPos(entityId).orElse(-1);
        }

        component::EntityComponentManager::SharedPtr manager;
    public:
        /*explicit ViewCore(const component::ComponentManagerImpl<Args...>& compManager) {
            pointerTuple = std::move(compManager.ptrTuple);
        }*/
        explicit ViewCore(component::EntityComponentManager::SharedPtr compManager) :
            /*pointerTuple{compManager->ptrTuple},*/ manager(std::move(compManager)) {};
        explicit ViewCore () : manager{} {};
        //explicit ViewCore(std::tuple<add_pointer<Args>...> tup) : pointerTuple{tup} {};
        friend class ViewBaseImpl;
        friend class ViewPropertyRotation;
    };

    class ViewBaseImpl {
    protected:
        ViewCore viewCore;
        template <typename T>
        T* getPointer () {
            return viewCore.getPointer<T>();
        }

        std::size_t getPos (component::EntityId entityId) {
            return viewCore.getPos(entityId);
        }
        ViewBaseImpl() : viewCore() {};
        explicit ViewBaseImpl(ViewCore core) : viewCore{std::move(core)} {}
    };

/*#define VIEW_GET_PROPERTY_PTR (MANAGER_PTR, COMP_ID, STRUCT_TYPE, STRUCT_MEMBER, ID) \
    (((std::byte*)MANAGER_PTR.getObjectDataPtr<add_pointer<STRUCT_TYPE>>(COMP_ID, ID)) + offsetof(STRUCT_TYPE, STRUCT_MEMBER))*/
#define VIEW_GET_PROPERTY_PTR(STRUCT_TYPE, MEMBER_NAME, ENTITY_ID) \
    (&(getPointer<STRUCT_TYPE>()[getPos(ENTITY_ID)].MEMBER_NAME))

    template <typename T, typename = typename T::args>
    struct ViewBaseWrap;

    template <typename T, typename ...Args>
    struct ViewBaseWrap <T, meta::type_list<Args...>> {
        using type = ViewBaseImpl;
    };

    template <typename T, typename = typename T::args>
    struct ViewCoreListImpl;

    template <typename T, typename ...Args>
    struct ViewCoreListImpl <T, meta::type_list<Args...>> {
        using type = ViewCore;
    };

    template <typename T>
    using ViewCoreList = typename ViewCoreListImpl<T>::type;

    template <typename T>
    using ViewBase = typename ViewBaseWrap<T>::type;

    class ViewPropertyRotation : public ViewPropertyCustom<float, ViewPropertyRotation> {
    private:
        float newVal{};
        component::RotationComponent* compPtr{};
        component::EntityComponentManager::SharedPtr manager{};
        event::EventBus::SharedPtr eventBus{};
        component::EntityId entityId{};
    public:
        explicit ViewPropertyRotation (ViewCoreList<component::entity_list> core,
                                       event::EventBus::SharedPtr bus, component::EntityId _entityId) :
                ViewPropertyCustom<float, ViewPropertyRotation>(&newVal), compPtr(core.getPointer<component::RotationComponent>() + core.getPos(_entityId)),
                newVal((core.getPointer<component::RotationComponent>() + core.getPos(_entityId))->rotation),
                manager(core.manager), eventBus(std::move(bus)), entityId(_entityId) {};
        ViewPropertyRotation () : ViewPropertyCustom<float, ViewPropertyRotation>(nullptr) {}
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

    class ViewPropertyScale {
    protected:
        glm::mat2* matPtr;
    public:
        ViewPropertyScale () : matPtr(nullptr) {};
        explicit ViewPropertyScale (glm::mat2* _ptr) : matPtr{_ptr} {}

        virtual void scaleBy (float xScale, float yScale) {
            glm::mat2 scaleMat = {{xScale, 0}, {0, yScale}};

            *matPtr = scaleMat * *matPtr;
        }

        virtual glm::vec2 getScale () {
            return {glm::sqrt((*matPtr)[0][0] * (*matPtr)[0][0] + (*matPtr)[0][1] * (*matPtr)[0][1]),
                    glm::sqrt((*matPtr)[1][0] * (*matPtr)[1][0] + (*matPtr)[1][1] * (*matPtr)[1][1])};
        }

        virtual void setScale (float xScale, float yScale) {
            auto scale = getScale();
            *matPtr = glm::mat2({xScale, 0}, {0, yScale}) * glm::mat2({(*matPtr)[0] / scale[0], (*matPtr)[1] / scale[1]});
        }
    };

    class ViewPropertyCollisionBox : ViewPropertyScale {
    private:
        float* radiusPtr;
        void calcRadius () {
            auto vec = (*matPtr)[0] + (*matPtr)[1];
            *radiusPtr = glm::sqrt(vec[0] * vec[0] + vec[1] * vec[1]);
        }
    public:
        ViewPropertyCollisionBox () : ViewPropertyScale(), radiusPtr(nullptr) {}
        explicit ViewPropertyCollisionBox (glm::mat2* matPtr, float* _radiusPtr) : ViewPropertyScale(matPtr), radiusPtr(_radiusPtr) {}


        void scaleBy (float xScale, float yScale) override {
            ViewPropertyScale::scaleBy(xScale, yScale);
            calcRadius();
        }

        void setScale (float xScale, float yScale) override {
            ViewPropertyScale::setScale(xScale, yScale);
            calcRadius();
        }

    };

    class EntityView : public ViewBase<component::entity_list> {
    public:
        const component::EntityId entityId;
        event::EventBus::SharedPtr eventBus;
        ViewProperty<game::AbstractEntity*> entity;

        ViewProperty<std::shared_ptr<game::EntityController>> controller;

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

        ViewPropertyScale modelScale;
        ViewPropertyCollisionBox hitboxScale;

        EntityView () : ViewBase<component::entity_list>(),
            entityId(0, 0),
            eventBus{nullptr},
            rotation(),
            entity(),
            controller(),
            position(),
            velocity(),
            acceleration(),
            constantFriction(),
            linearFriction(),
            model(),

            collisionLayers(),
            resolveLayers(),
            eventLayers() {};
        // TODO: add scaling
        EntityView (const ViewCoreList<component::entity_list>& core, component::EntityId id, const event::EventBus::SharedPtr& bus) : ViewBase<component::entity_list>(core),
            entityId(id),
            eventBus(bus),
            rotation(core, bus, id),
            entity(getPointer<game::AbstractEntity*>() +  getPos(id)),
            controller(getPointer<std::shared_ptr<game::EntityController>>() + getPos(id)),
            position(ViewProperty(VIEW_GET_PROPERTY_PTR(component::EntityMainComponent, pos, id))),
            velocity(ViewProperty(VIEW_GET_PROPERTY_PTR(component::EntityMainComponent, vel, id))),
            acceleration(ViewProperty(VIEW_GET_PROPERTY_PTR(component::EntityMainComponent, acc, id))),

            constantFriction(ViewProperty(VIEW_GET_PROPERTY_PTR(component::EntityMainComponent, constFriction, id))),
            linearFriction(ViewProperty(VIEW_GET_PROPERTY_PTR(component::EntityMainComponent, linFriction, id))),

            model(ViewProperty(getPointer<graphics::FixedModel>() + getPos(id))),

            collisionLayers(ViewProperty(VIEW_GET_PROPERTY_PTR(physics::CollisionComponent, layers, id))),
            resolveLayers(ViewProperty(VIEW_GET_PROPERTY_PTR(physics::CollisionComponent, resolveLayers, id))),
            eventLayers(ViewProperty(VIEW_GET_PROPERTY_PTR(physics::CollisionComponent, layers, id))),
            modelScale(VIEW_GET_PROPERTY_PTR(graphics::AbsolutePosition, transform, id)),
            hitboxScale(VIEW_GET_PROPERTY_PTR(physics::CollisionComponent, bbMap, id),
                        VIEW_GET_PROPERTY_PTR(physics::CollisionComponent, outerRadius, id)){};

        EntityView withId (component::EntityId id) {
            return {viewCore, id, eventBus};
        }
    };
}
