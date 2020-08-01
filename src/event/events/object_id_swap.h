#include <unordered_map>
#include <string>
#include "event/event.h"
#ifndef ENTITY_ID_SWAP_H
#define ENTITY_ID_SWAP_H
namespace event {
    static std::unordered_map<std::string, std::string> OBJECT_SWAP_EVENT_TYPES;
    template<typename T>
    class ObjectIdSwapEvent : public Event<ObjectIdSwapEvent<T>> {
        public:
        int oldId{};
        int newId{};
        std::string objectType;
        std::string name;
        ObjectIdSwapEvent () {
            objectType = T::getObjectType();
            if (OBJECT_SWAP_EVENT_TYPES.count(objectType) == 0) {
                OBJECT_SWAP_EVENT_TYPES[objectType] = "'" + objectType + "' id_swap";
            }
            name = OBJECT_SWAP_EVENT_TYPES[objectType];
            oldId = 0;
            newId = 0;
        }
        ObjectIdSwapEvent (int oldId, int newId) {
            objectType = T::getObjectType();
            this->oldId = oldId;
            this->newId = newId;
        };
    };
}
#endif