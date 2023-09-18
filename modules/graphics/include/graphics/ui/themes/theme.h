#pragma once

#include <memory>

#include "forward.h"

#include "util/map.h"
#include "util/optional.h"

namespace util {
    class DataValue;
}

namespace graphics::ui {
    class Theme {
    private:
        util::Map<std::string, std::unique_ptr<ThemeClass>> themeClassMap;
        std::string themeName;
    public:
        explicit Theme (const util::DataValue& themeData);

        const std::string& getThemeName ();

        util::Optional<ThemeClass*> getThemeClass (const std::string& classId);

        std::string getAbsoluteId (const std::string& relativeId, const std::string& classPrefix);

        ThemeClass* addThemeClass (const std::string& classId, std::unique_ptr<ThemeClass> themeClass);
    };

    std::unique_ptr<Theme> loadTheme (std::istream& stream);
}