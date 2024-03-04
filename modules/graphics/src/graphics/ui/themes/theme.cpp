#include "graphics/ui/themes/theme.h"

#include "graphics/detail/loggers.h"
#include "graphics/ui/themes/theme_class.h"

using namespace phenyl::graphics::ui;

static phenyl::Logger LOGGER{"UI_THEME", phenyl::graphics::detail::GRAPHICS_LOGGER};

Theme::Theme (const util::DataValue& themeData) {
    const auto& obj = themeData.get<util::DataObject>();

    themeName = obj.at("theme_name").get<std::string>();

    for (auto [id, data] : obj.kv()) {
        if (id == "theme_name") {
            continue;
        } else if (!data.is<util::DataObject>()) {
            PHENYL_LOGE(LOGGER, R"(Data of class "{}" in theme "{}" is not an object!)", id, themeName);
        } else {
            std::unique_ptr<ThemeClass> themeClass = std::make_unique<ThemeClass>(id, this, data.get<util::DataObject>(), "");

            themeClassMap[id] = std::move(themeClass);
        }
    }

    if (!themeClassMap.contains("default")) {
        PHENYL_LOGE(LOGGER, "No default class found in theme \"{}\"!", themeName);
        themeClassMap["default"] = std::make_unique<ThemeClass>("default", this, util::DataObject(), "");
    }

    for (auto [id, themeClass] : themeClassMap.kv()) {
        themeClass->setParent();
    }
}

phenyl::util::Optional<ThemeClass*> phenyl::graphics::ui::Theme::getThemeClass (const std::string& classId) {
    if (themeClassMap.contains(classId)) {
        return {themeClassMap[classId].get()};
    }
    return util::NullOpt;
}

std::string Theme::getAbsoluteId (const std::string& relativeId, const std::string& classPrefix) {
    if (relativeId[0] == '.') {
        return classPrefix + relativeId;
    }
    return relativeId;
}

const std::string& Theme::getThemeName () {
    return themeName;
}

ThemeClass* Theme::addThemeClass (const std::string& classId, std::unique_ptr<ThemeClass> themeClass) {
    themeClassMap[classId] = std::move(themeClass);
    return themeClassMap[classId].get();
}

std::unique_ptr<Theme> phenyl::graphics::ui::loadTheme (std::istream& stream) {
    auto data = util::parseFromStream(stream);
    if (data.empty()) {
        PHENYL_LOGE(LOGGER, "Failed to load theme!");
        return nullptr;
    }

    return std::make_unique<Theme>(data);
}
