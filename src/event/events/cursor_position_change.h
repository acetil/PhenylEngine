#include "event/event.h"
#include "graphics/maths_headers.h"
#ifndef CURSOR_POSITION_CHANGE_H
#define CURSOR_POSITION_CHANGE_H
namespace event {
    class CursorPosChangeEvent : public Event<CursorPosChangeEvent, false> {
    public:
        std::string name = "cursor_pos_change";
        glm::vec2 windowPos = {0, 0};
        glm::vec2 screenPos = {0, 0};
        CursorPosChangeEvent() = default;
        CursorPosChangeEvent (glm::vec2 _windowPos, glm::vec2 _screenPos) :
            windowPos(_windowPos), screenPos{_screenPos} {};
    };
}
#endif
