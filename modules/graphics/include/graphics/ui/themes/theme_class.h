#pragma once

#include "forward.h"

#include "util/map.h"
#include "util/optional.h"
#include "util/data.h"

namespace graphics::ui {
    class ThemeClass {
    private:
        util::Map<std::string, ThemeClass*> subClasses;
        util::Map<std::string, util::DataValue> properties;

        Theme* theme;
        ThemeClass* parent;
        std::string parentId;
        std::string classId;
        util::Optional<ThemeClass*> getChild (const std::string& childId);
    public:
        ThemeClass (std::string classId, Theme* theme, const util::DataObject& classData, const std::string& classPrefix);

        util::Optional<ThemeClass*> getSubClass (const std::string& subClassId);

        void setParent ();

        template <typename T>
        util::Optional<T> getProperty (const std::string& propertyId) {
            return getPropertyValue(propertyId)
                .then([] (const util::DataValue& val) {
                     T prop;
                     if (val.getValue(prop)) {
                         return util::Optional<T>(prop);
                     } else {
                         return util::NullOpt;
                     }
                });
        }

        util::Optional<const util::DataValue&> getPropertyValue (const std::string& propertyId);

        bool hasProperty (const std::string& propertyId);

        friend class Theme;
    };
}