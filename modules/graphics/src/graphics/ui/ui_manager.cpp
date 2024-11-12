#include "common/assets/assets.h"

#include "graphics/ui/ui_manager.h"

#include "common/input/game_input.h"
#include "graphics/renderer.h"
#include "graphics/ui/widgets/event.h"

#include "resources/themes/default_theme.json.h"

#include "logging/logging.h"

using namespace phenyl::graphics;

static phenyl::Logger LOGGER{"UI_MANAGER", detail::GRAPHICS_LOGGER};

UIManager::UIManager (common::GameInput& input) : selectAction{input.addAction("ui_select")}, mousePos{input.addAxis2D("ui_mouse")} {
    rootWidget = std::make_unique<RootWidget>();

    input.addActionBinding("ui_select", "mouse.button_left");
    input.addAxis2DBinding("ui_mouse", "mouse.mouse_pos");
}

void UIManager::renderUI (Canvas& canvas) {
    rootWidget->measure(WidgetConstraints{
        .maxSize = canvas.resolution()
    });
    rootWidget->render(canvas);
}

void UIManager::updateUI () {
    PHENYL_TRACE(LOGGER, "Updating UI");
    rootWidget->pointerUpdate(mousePos.value());

    bool newMouse = selectAction.value();
    if (newMouse != mouseDown) {
        if (newMouse) {
            focusedWidget = rootWidget->pick(mousePos.value());
            if (focusedWidget) {
                PHENYL_LOGD(LOGGER, "Received mouse press");
                focusedWidget->raise(UIEvent{MousePressEvent{}});
            }
        } else {
            if (focusedWidget) {
                PHENYL_LOGD(LOGGER, "Received mouse release");
                focusedWidget->raise(UIEvent{MouseReleaseEvent{}});
            }
        }
    }
    mouseDown = newMouse;

    rootWidget->update();
}

std::string_view UIManager::getName () const noexcept {
    return "UIManager";
}