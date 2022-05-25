#pragma once

#include "event/event.h"

namespace event {
    class ChangeThemeEvent : public Event<ChangeThemeEvent> {
    public:
        inline static const std::string name = "theme_change";
        const std::string& themeName;

        explicit ChangeThemeEvent (const std::string& _themeName) : themeName{_themeName} {}
    };
}