#include <unordered_map>
#include <string>
#include "event/event.h"
#ifndef ENTITY_ID_SWAP_H
#define ENTITY_ID_SWAP_H
namespace event {
    //static std::unordered_map<std::string, std::string> OBJECT_SWAP_EVENT_TYPES;
    template<typename T>
    class ObjectIdSwapEvent : public Event<ObjectIdSwapEvent<T>, true> {
        public:
        int oldId{};
        int newId{};
        std::string objectType;
        inline static const std::string name = T::getObjectType() + " id swap";
        ObjectIdSwapEvent () {
            oldId = 0;
            newId = 0;
        }
        ObjectIdSwapEvent (int oldId, int newId) {
            this->oldId = oldId;
            this->newId = newId;
        };
    };
}
#endif