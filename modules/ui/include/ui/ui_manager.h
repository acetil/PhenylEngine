#pragma once

#include "core/input/axis_action.h"
#include "core/input/input_action.h"
#include "core/iresource.h"
#include "graphics/canvas/canvas.h"
#include "graphics/font/glyph_atlas.h"
#include "widgets/root.h"
#include "widgets/widget.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace phenyl::graphics {
class UIManager : public core::IResource {
public:
    UIManager (core::GameInput& input);

    void renderUI (Canvas& canvas);
    void updateUI ();

    std::string_view getName () const noexcept override;

    [[nodiscard]] RootWidget& root () const noexcept {
        return *m_rootWidget;
    }

private:
    bool m_mouseDown = false;
    std::unique_ptr<RootWidget> m_rootWidget;
    Widget* m_focusedWidget = nullptr;

    core::InputAction m_selectAction;
    core::Axis2DInput m_mousePos;
};
} // namespace phenyl::graphics
