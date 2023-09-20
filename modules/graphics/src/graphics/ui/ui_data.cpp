#include "graphics/ui/ui_data.h"

using namespace phenyl::graphics::ui;

bool phenyl::graphics::ui::phenyl_from_data (const util::DataValue& dataVal, Axis& val) {
    if (!dataVal.is<std::string>()) {
        return false;
    }

    const auto& valStr = dataVal.get<std::string>();

    if (valStr == "up") {
        val = Axis::UP;
        return true;
    } else if (valStr == "down") {
        val = Axis::DOWN;
        return true;
    } else if (valStr == "left") {
        val = Axis::LEFT;
        return true;
    } else if (valStr == "right") {
        val = Axis::RIGHT;
        return true;
    }

    return false;
}

phenyl::util::DataValue phenyl::graphics::ui::phenyl_to_data (const Axis& val) {
    switch (val) {
        case Axis::UP:
            return util::DataValue("up");
        case Axis::DOWN:
            return util::DataValue("down");
        case Axis::LEFT:
            return util::DataValue("left");
        case Axis::RIGHT:
            return util::DataValue("right");
    }
    return util::DataValue{};
}

bool phenyl::graphics::ui::phenyl_from_data (const util::DataValue& dataVal, FlexAlign& val) {
    if (!dataVal.is<std::string>()) {
        return false;
    }

    const auto& valStr = dataVal.get<std::string>();

    if (valStr == "centre") {
        val = FlexAlign::CENTRE;
        return true;
    } else if (valStr == "start") {
        val = FlexAlign::START;
        return true;
    } else if (valStr == "end") {
        val = FlexAlign::END;
        return true;
    }

    return false;
}

phenyl::util::DataValue phenyl::graphics::ui::phenyl_to_data (const FlexAlign& val) {
    switch (val) {
        case FlexAlign::CENTRE:
            return util::DataValue("centre");
        case FlexAlign::START:
            return util::DataValue("start");
        case FlexAlign::END:
            return util::DataValue("end");
    }

    return util::DataValue{};
}

bool phenyl::graphics::ui::phenyl_from_data (const util::DataValue& dataVal, FlexJustify& val) {
    if (!dataVal.is<std::string>()) {
        return false;
    }

    const auto& valStr = dataVal.get<std::string>();

    if (valStr == "centre") {
        val = FlexJustify::CENTRE;
        return true;
    } else if (valStr == "none") {
        val = FlexJustify::NONE;
        return true;
    } else if (valStr == "space_evenly") {
        val = FlexJustify::SPACE_EVENLY;
        return true;
    } else if (valStr == "space_around") {
        val = FlexJustify::SPACE_AROUND;
        return true;
    } else if (valStr == "space_between") {
        val = FlexJustify::SPACE_BETWEEN;
        return true;
    }

    return false;
}

phenyl::util::DataValue phenyl::graphics::ui::phenyl_to_data (const FlexJustify& val) {
    switch (val) {
        case FlexJustify::CENTRE:
            return util::DataValue("centre");
        case FlexJustify::NONE:
            return util::DataValue("none");
        case FlexJustify::SPACE_EVENLY:
            return util::DataValue("space_evenly");
        case FlexJustify::SPACE_BETWEEN:
            return util::DataValue("space_between");
        case FlexJustify::SPACE_AROUND:
            return util::DataValue("space_around");
    }

    return util::DataValue{};
}

bool phenyl::graphics::ui::phenyl_from_data (const util::DataValue& dataVal, FloatAnchor& val) {
    if (!dataVal.is<std::string>()) {
        return false;
    }

    const auto& valStr = dataVal.get<std::string>();

    if (valStr == "centre") {
        val = FloatAnchor::FLOAT_CENTRE;
        return true;
    } else if (valStr == "start") {
        val = FloatAnchor::FLOAT_START;
        return true;
    } else if (valStr == "end") {
        val = FloatAnchor::FLOAT_END;
        return true;
    } else if (valStr == "auto") {
        val = FloatAnchor::AUTO;
    }

    return false;
}

phenyl::util::DataValue phenyl::graphics::ui::phenyl_to_data (const FloatAnchor& val) {
    switch (val) {
        case FloatAnchor::FLOAT_CENTRE:
            return util::DataValue("centre");
        case FloatAnchor::FLOAT_START:
            return util::DataValue("start");
        case FloatAnchor::FLOAT_END:
            return util::DataValue("end");
        case FloatAnchor::AUTO:
            return util::DataValue("auto");
    }

    return util::DataValue{};
}