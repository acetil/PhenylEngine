#include "graphics/ui/themes/theme_class.h"

#include <utility>
#include "graphics/ui/themes/theme.h"

using namespace graphics::ui;

ThemeClass::ThemeClass (std::string _classId, Theme* _theme, const util::DataObject& classData, const std::string& classPrefix) : classId{std::move(_classId)},
    theme{_theme}, parent{nullptr} {

    if (classData.contains("parentId")) {
        parentId = theme->getAbsoluteId(classData.at("parentId").toString(), classPrefix);
    } else {
        parentId = "default";
    }

    if (classData.contains("subclasses")) {
        auto& subclassesVal = classData.at("subclasses");
        if (!subclassesVal.is<util::DataObject>()) {
            logging::log(LEVEL_ERROR, "Subclasses property of class {} in theme {} is not object!", classId, theme->getThemeName());
        } else {
            for (auto [id, data] : subclassesVal.get<util::DataObject>().kv()) {
                if (!data.is<util::DataObject>()) {
                    logging::log(LEVEL_ERROR, "Class {} data of theme {} is not an object!", classId + "." + id, theme->getThemeName());
                } else {
                    subClasses[id] = theme->addThemeClass(classId + "." + id, std::make_unique<ThemeClass>(classId + "." + id, theme, data.get<util::DataObject>(), classId));
                }
            }
        }
    }

    for (auto [propId, propData] : classData.kv()) {
        if (propId == "parentId" || propId == "subclasses") {
            continue;
        }

        properties[propId] = propData;
    }
}

util::Optional<ThemeClass*> ThemeClass::getSubClass (const std::string& subClassId) {
    return theme->getThemeClass(theme->getAbsoluteId(subClassId, classId));
}

util::Optional<ThemeClass*> ThemeClass::getChild (const std::string& childId) {
    if (subClasses.contains(childId)) {
        return {subClasses[childId]};
    }
    return util::NullOpt;
}

util::Optional<const util::DataValue&> ThemeClass::getPropertyValue (const std::string& propertyId) {
    if (properties.contains(propertyId)) {
        return util::Optional<const util::DataValue&>(properties.at(propertyId));
    } else if (parent) {
        return parent->getPropertyValue(propertyId)
            .thenIfPresent([&] (const util::DataValue& val) {
                this->properties[propertyId] = val;
            });
    } else {
        return util::NullOpt;
    }
}

void ThemeClass::setParent () {
    /*auto parentClassOpt = theme->getThemeClass(parentId);

    parentClassOpt.ifPresent([this] (ThemeClass* parentClass) {
       this->parentId = parentClass;
    });

    parentClassOpt.ifNotPresent([this] () {
        logging::log(LEVEL_ERROR, "Could not find parentId class {} for theme class {}!", parentId, classId);
        this->parentId = theme->getThemeClass("default").orElse(nullptr);
    });*/

    if (classId == parentId) {
        return;
    }

    theme->getThemeClass(parentId)
        .thenIfPresent([this] (ThemeClass* parentClass) {
            this->parent = parentClass;
        })
        .ifNotPresent([this] () {
            logging::log(LEVEL_ERROR, "Could not find parentId class {} for theme class {}!", parentId, classId);
            this->parent = theme->getThemeClass("default").orElse(nullptr);
        });
}

bool ThemeClass::hasProperty (const std::string& propertyId) {
    return properties.contains(propertyId) || (parent && parent->hasProperty(propertyId));
}
