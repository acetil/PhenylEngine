#pragma once

#include "event/event.h"

namespace event {
    class ReloadThemeEvent : public Event<ReloadThemeEvent> {
    public:
        inline static const std::string name = "theme_reload";
    };
}