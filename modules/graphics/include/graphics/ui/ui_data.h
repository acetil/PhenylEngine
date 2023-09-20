#pragma once

#include "util/data_funcs.h"
#include "ui_defs.h"
#include "nodes/ui_anchor.h"
#include "nodes/ui_flexbox.h"

namespace phenyl::graphics::ui {
    bool phenyl_from_data (const util::DataValue& dataVal, Axis& val);
    util::DataValue phenyl_to_data (const Axis& val);

    bool phenyl_from_data (const util::DataValue& dataVal, FloatAnchor& val);
    util::DataValue phenyl_to_data (const FloatAnchor& val);

    bool phenyl_from_data (const util::DataValue& dataVal, FlexJustify& val);
    util::DataValue phenyl_to_data (const FlexJustify& val);

    bool phenyl_from_data (const util::DataValue& dataVal, FlexAlign& val);
    util::DataValue phenyl_to_data (const FlexAlign& val);
}