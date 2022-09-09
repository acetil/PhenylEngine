#include "graphics/ui/themes/theme_properties.h"
#include "graphics/ui/themes/theme.h"
#include "graphics/ui/themes/theme_class.h"
#include "logging/logging.h"
#include <utility>

using namespace graphics::ui;

ThemeProperties::ThemeProperties (std::string _className, std::string _fallbackName, std::string _classPrefix) :
    className{std::move(_className)}, fallbackName{std::move(_fallbackName)}, classPrefix{std::move(_classPrefix)} {}

void ThemeProperties::applyTheme (Theme* _theme) {
    theme = _theme;
    fallbackClass = theme->getThemeClass(theme->getAbsoluteId(fallbackName, classPrefix))
                         .orOpt([this] () {return theme->getThemeClass("default");})
                         .orElse(nullptr);
    themeClass = theme->getThemeClass(theme->getAbsoluteId(className, classPrefix)).orElse(fallbackClass);
}

bool ThemeProperties::hasProperty (const std::string& propertyId) const {
    return themeClass->hasProperty(propertyId) || fallbackClass->hasProperty(propertyId);
}

util::Optional<const util::DataValue&> ThemeProperties::getPropertyValue (const std::string& propertyId) const {
    if (!hasProperty(propertyId)) {
        //logging::log(LEVEL_ERROR, "Property {} not in theme class {}!", propertyId, className);
        return util::NullOpt;
    }

    return themeClass->getPropertyValue(propertyId).orOpt([this, &propertyId] () {
        return fallbackClass->getPropertyValue(propertyId);
    });
}