#include <unordered_map>
#include <string>
#include <vector>
#include "event/events/object_id_swap.h"

#include <cmath>
#include "event/event.h"
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
    struct Component {
        size_t size{};
        size_t sizeElement{};

        virtual void* getData () = 0;
        virtual ~Component() {};
    };
    template <class T, int sizeE = 1>
    struct ComponentImpl  : public Component{
    private:
        T* data;
    public:
        ComponentImpl (int maxEntities) {
            data = new T[maxEntities * sizeE];
            this->size = sizeE * sizeof(T);
            sizeElement = sizeE;
        }
        void* getData () override  {
            return (void*) data;
        };
        ~ComponentImpl () {
            delete data;
        }
    };
    template <class C>
    class ComponentManager {
        private:
        std::unordered_map<std::string, int> componentIds;
        std::vector<Component*> components;
        id_type_t numEntities;
        int maxEntities;
        event::EventBus* bus;
        public:
        ComponentManager(int maxEntities, event::EventBus* bus, std::string defaultPtr) {
            this->maxEntities = fmin(maxEntities, MAX_COMPONENT_ENTITIES);
            this->numEntities = 0;
            this->bus = bus;
            addComponent<C>(defaultPtr);
        };
        ~ComponentManager() {
            components.clear();
        };
        template <typename T>
        void applyFunc (void func(T*, int, int), int compId) {
            func((T*)components[compId]->getData(), numEntities, components[compId]->sizeElement);
        }
        template <typename T, typename A>
        void applyFunc (void func(T*, int, int, A), int compId, A extra) {
            func((T*)components[compId]->getData(), numEntities, components[compId]->sizeElement, extra);
        }
        template <typename T>
        void applyFuncThreaded (void func(T*, int, int), int compId, int numThreads) {
            applyFunc(func, compId); // TODO: update
        }
        template <typename T, typename A>
        void applyFuncThreaded (void func(T*, int, int , A), int compId, int numThreads, A extra) {
            applyFunc(func, compId, extra); // TODO: same as above
        }
        template<typename T>
        void applyFunc (void func(T*, int, int, int, int), int compId) {
            func((T*)components[compId]->getData(), 0, numEntities, 1, components[compId]->sizeElement);
        }
        template<typename T>
        void applyFuncThreaded (void func(T*, int, int, int, int), int compId, int numThreads) {
            applyFunc(func, compId); // TODO: see above
        }
        template<typename T, typename A>
        void applyFunc (void func(T*, int, int, int, int, A), int compId, A extra) {
            func((T*)components[compId]->getData(), 0, numEntities, 1, components[compId]->sizeElement, extra);
        }
        template<typename T, typename A>
        void applyFuncThreaded (void func(T*, int, int, int, int, A), int compId, A extra) {
            applyFunc(func, compId, extra); // TODO: see above
        }
        template <typename T, int size = 1>
        int addComponent (std::string name) {
            if (componentIds.count(name) > 0) {
                return -1;
            }
            components.push_back(new ComponentImpl<T, size>(maxEntities));
            componentIds[name] = components.size() - 1;
            return componentIds[name];
        }
        template <typename T>
        T* getComponent (int compId) {
            return (T*) components[compId]->getData();
        }
        template <typename T>
        T* getComponent (std::string name) {
            return getComponent<T>(componentIds[name]);
        }
        int getComponentId (std::string name) {
            return componentIds[name];
        }
        template <typename T>
        T getObjectData (int compId, int entityId) {
            // not the recommended way to access component data, use applyFunc where possible
            return ((T*)components[compId]->getData())[entityId * components[compId]->sizeElement];
        };
        template <typename T>
        T* getObjectDataPtr (int compId, int entityId) {
            return ((T*)components[compId]->getData()) + entityId * components[compId]->sizeElement;
        }
        int getElementSize (int compId) {
            return components[compId]->sizeElement;
        }
        id_type_t getNumObjects () {
            return numEntities;
        };
        id_type_t addObject (C object) {
            logging::logf(LEVEL_DEBUG, "Num objects: %d", numEntities);
            getComponent<C>(0)[numEntities] = object;
            return numEntities++;
        };
        void removeObject (id_type_t entityId) {
            if (entityId >= numEntities) {
                logging::logf(LEVEL_ERROR, "Attempted to remove object id %d, only %d object components!", entityId, numEntities);
                return;
            }
            id_type_t oldId = --numEntities;

            // char* to avoid the compiler shouting at me
            for (Component* c : components) {
                memcpy((char*)c->getData() + entityId * c->size, (char*)c->getData() + oldId * c->size, c->size);
            }
            bus->raiseEvent(new event::ObjectIdSwapEvent<C>(oldId, entityId));
        };
    };
}
#endif