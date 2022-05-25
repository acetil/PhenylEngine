#include "graphics/ui/themes/theme.h"
#include "graphics/ui/themes/theme_class.h"

using namespace graphics::ui;

Theme::Theme (const util::DataValue& themeData) {
    const auto& obj = themeData.get<util::DataObject>();

    themeName = obj.at("theme_name").get<std::string>();

    for (auto [id, data] : obj.kv()) {
        if (id == "theme_name") {
            continue;
        } else if (!data.is<util::DataObject>()) {
            logging::log(LEVEL_ERROR, R"(Data of class "{}" in theme "{}" is not an object!)", id, themeName);
        } else {
            std::unique_ptr<ThemeClass> themeClass = std::make_unique<ThemeClass>(id, this, data.get<util::DataObject>(), "");

            themeClassMap[id] = std::move(themeClass);
        }
    }

    if (!themeClassMap.contains("default")) {
        logging::log(LEVEL_ERROR, "No default class found in theme \"{}\"!", themeName);
        themeClassMap["default"] = std::make_unique<ThemeClass>("default", this, util::DataObject(), "");
    }

    for (auto [id, themeClass] : themeClassMap.kv()) {
        themeClass->setParent();
    }
}

util::Optional<ThemeClass*> graphics::ui::Theme::getThemeClass (const std::string& classId) {
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

std::unique_ptr<Theme> graphics::ui::loadTheme (const std::string& themePath) {
    util::DataValue data = util::parseFromFile(themePath);

    return std::make_unique<Theme>(data);
}
