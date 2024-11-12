#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <vector>

#include "graphics/font/glyph_atlas.h"
#include "common/input/axis_action.h"
#include "common/input/input_action.h"
#include "graphics/canvas/canvas.h"
#include "runtime/iresource.h"
#include "widgets/root.h"
#include "widgets/widget.h"

namespace phenyl::graphics {
    class UIManager : public runtime::IResource {
    private:
        bool mouseDown = false;
        std::unique_ptr<RootWidget> rootWidget;
        Widget* focusedWidget = nullptr;

        common::InputAction selectAction;
        common::Axis2DInput mousePos;
    public:
        UIManager (common::GameInput& input);

        void renderUI (Canvas& canvas);
        void updateUI ();

        std::string_view getName() const noexcept override;

        [[nodiscard]] RootWidget& root () const noexcept {
            return *rootWidget;
        }
    };
}
