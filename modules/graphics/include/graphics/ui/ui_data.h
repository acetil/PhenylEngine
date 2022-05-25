#pragma once

#include "util/data_funcs.h"
#include "ui_defs.h"
#include "components/ui_anchor.h"
#include "components/ui_flexbox.h"

namespace graphics::ui {
    bool fromdata (const util::DataValue& dataVal, Axis& val);
    util::DataValue todata (const Axis& val);

    bool fromdata (const util::DataValue& dataVal, FloatAnchor& val);
    util::DataValue todata (const FloatAnchor& val);

    bool fromdata (const util::DataValue& dataVal, FlexJustify& val);
    util::DataValue todata (const FlexJustify& val);

    bool fromdata (const util::DataValue& dataVal, FlexAlign& val);
    util::DataValue todata (const FlexAlign& val);
}

namespace util {
    template <>
    inline bool fromdata (const DataValue& dataVal, graphics::ui::Axis& val) {
        return graphics::ui::fromdata(dataVal, val);
    }
    template <>
    inline DataValue todata (const graphics::ui::Axis& val) {
        return todata(val);
    }

    template <>
    inline bool fromdata (const DataValue& dataVal, graphics::ui::FloatAnchor& val) {
        return graphics::ui::fromdata(dataVal, val);
    }
    template <>
    inline DataValue todata (const graphics::ui::FloatAnchor& val) {
        return todata(val);
    }

    template <>
    inline bool fromdata (const DataValue& dataVal, graphics::ui::FlexJustify& val) {
        return graphics::ui::fromdata(dataVal, val);
    }
    template <>
    inline DataValue todata (const graphics::ui::FlexJustify& val) {
        return todata(val);
    }

    template <>
    inline bool fromdata (const DataValue& dataVal, graphics::ui::FlexAlign& val) {
        return graphics::ui::fromdata(dataVal, val);
    }
    template <>
    inline DataValue todata (const graphics::ui::FlexAlign& val) {
        return todata(val);
    }
}