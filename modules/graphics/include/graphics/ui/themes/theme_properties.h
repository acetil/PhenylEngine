#pragma once

#include <string>

#include "forward.h"
#include "util/optional.h"
#include "util/data.h"

namespace graphics::ui {
    class ThemeProperties {
    private:
        Theme* theme = nullptr;
        ThemeClass* themeClass = nullptr;
        ThemeClass* fallbackClass = nullptr;
        std::string className;
        std::string fallbackName;
        std::string classPrefix;

    public:
        ThemeProperties (std::string className, std::string fallbackName, std::string classPrefix = "");

        void applyTheme (Theme* theme);

        [[nodiscard]] bool hasProperty (const std::string& propertyId) const;

        [[nodiscard]] util::Optional<const util::DataValue&> getPropertyValue (const std::string& propertyId) const;

        template <typename T>
        util::Optional<T> getProperty (const std::string& propertyId) const {
            return getPropertyValue(propertyId).then([] (const util::DataValue& val) {
                T obj;
                if (val.getValue(obj)) {
                    return util::Optional<T>{std::move(obj)};
                } else {
                    return util::Optional<T>();
                }
                /*if (util::phenyl_from_data(val, obj)) {
                    return util::Optional<T>{obj};
                } else {
                    return util::Optional<T>{util::NullOpt};
                }*/
            });
        }
    };
}