#include "ui/ui_manager.h"

#include "core/assets/assets.h"
#include "core/input/game_input.h"
#include "graphics/backend/renderer.h"
#include "graphics/detail/loggers.h"
#include "logging/logging.h"
#include "ui/widgets/event.h"

using namespace phenyl::graphics;

static phenyl::Logger LOGGER{"UI_MANAGER", detail::GRAPHICS_LOGGER};

UIManager::UIManager (core::GameInput& input) :
    m_selectAction{input.addAction("ui_select")},
    m_mousePos{input.addAxis2D("ui_mouse")} {
    m_rootWidget = std::make_unique<RootWidget>();

    input.addActionBinding("ui_select", "mouse.button_left");
    input.addAxis2DBinding("ui_mouse", "mouse.mouse_pos");
}

void UIManager::renderUI (Canvas& canvas) {
    m_rootWidget->measure(WidgetConstraints{.maxSize = canvas.resolution()});
    m_rootWidget->render(canvas);
}

void UIManager::updateUI () {
    PHENYL_TRACE(LOGGER, "Updating UI");
    m_rootWidget->pointerUpdate(m_mousePos.value());

    bool newMouse = m_selectAction.value();
    if (newMouse != m_mouseDown) {
        if (newMouse) {
            m_focusedWidget = m_rootWidget->pick(m_mousePos.value());
            if (m_focusedWidget) {
                PHENYL_LOGD(LOGGER, "Received mouse press");
                m_focusedWidget->raise(UIEvent{MousePressEvent{}});
            }
        } else {
            if (m_focusedWidget) {
                PHENYL_LOGD(LOGGER, "Received mouse release");
                m_focusedWidget->raise(UIEvent{MouseReleaseEvent{}});
            }
        }
    }
    m_mouseDown = newMouse;

    m_rootWidget->update();
}

std::string_view UIManager::getName () const noexcept {
    return "UIManager";
}
