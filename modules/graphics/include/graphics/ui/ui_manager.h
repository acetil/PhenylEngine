#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <vector>

#include "graphics/font/glyph_atlas.h"
#include "core/input/axis_action.h"
#include "core/input/input_action.h"
#include "graphics/canvas/canvas.h"
#include "core/iresource.h"
#include "widgets/root.h"
#include "widgets/widget.h"

namespace phenyl::graphics {
    class UIManager : public core::IResource {
    private:
        bool mouseDown = false;
        std::unique_ptr<RootWidget> rootWidget;
        Widget* focusedWidget = nullptr;

        core::InputAction selectAction;
        core::Axis2DInput mousePos;
    public:
        UIManager (core::GameInput& input);

        void renderUI (Canvas& canvas);
        void updateUI ();

        std::string_view getName() const noexcept override;

        [[nodiscard]] RootWidget& root () const noexcept {
            return *rootWidget;
        }
    };
}
