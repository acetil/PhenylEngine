#include <unordered_map>
#include <string>
#include <vector>

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
        void* data;
        size_t size;
        size_t sizeElement;
    };
    class ComponentManager {
        private:
        std::unordered_map<std::string, int> componentIds;
        std::vector<Component> components;
        id_type_t numEntities;
        int maxEntities;
        event::EventBus* bus;
        public:
        ComponentManager(int maxEntities, event::EventBus* bus);
        ~ComponentManager();
        template <typename T>
        void applyFunc (void func(T*, int, int), int compId) {
            func((T*)components[compId].data, numEntities, components[compId].sizeElement);
        }
        template <typename T, typename A>
        void applyFunc (void func(T*, int, int, A), int compId, A extra) {
            func((T*)components[compId].data, numEntities, components[compId].sizeElement, extra);
        }
        template <typename T>
        void applyFuncThreaded (void func(T*, int, int), int compId, int numThreads) {
            applyFunc(func, compId); // TODO: update
        }
        template <typename T, typename A>
        void applyFuncThreaded (void func(T*, int, int , A), int compId, int numThreads, A extra) {
            applyFunc(func, compId, extra); // TODO: same as above
        }
        template <typename T, int size = 1>
        int addComponent (std::string name) {
            if (componentIds.count(name) > 0) {
                return -1;
            }
            components.push_back({new T[maxEntities * size], sizeof(T) * size, size});
            componentIds[name] = components.size() - 1;
            return componentIds[name];
        }
        template <typename T>
        T* getComponent (int compId) {
            return (T*) components[compId].data;
        }
        template <typename T>
        T* getComponent (std::string name) {
            return getComponent<T>(componentIds[name]);
        }
        int getComponentId (std::string name) {
            return componentIds[name];
        }
        template <typename T>
        T getEntityData (int compId, int entityId) {
            // not the recommended way to access component data, use applyFunc where possible
            return ((T*)components[compId].data)[entityId * components[compId].sizeElement];
        };
        template <typename T>
        T* getEntityDataPtr (int compId, int entityId) {
            return ((T*)components[compId].data) + entityId * components[compId].sizeElement;
        }
        int getElementSize (int compId) {
            return components[compId].sizeElement;
        }
        id_type_t getNumEntities ();
        id_type_t addEntity (game::AbstractEntity* entity);
        void removeEntity (id_type_t entityId);
    };
}
#endif