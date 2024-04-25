#include "common/assets/assets.h"

#include "graphics/ui/ui_manager.h"

#include "common/input/game_input.h"
#include "graphics/renderer.h"
#include "graphics/ui/nodes/ui_root.h"
#include "graphics/ui/themes/theme.h"
#include "graphics/ui/themes/theme_class.h"

#include "resources/themes/default_theme.json.h"

#include "logging/logging.h"

using namespace phenyl::graphics;

static phenyl::Logger LOGGER{"UI_MANAGER", detail::GRAPHICS_LOGGER};

UIManager::UIManager (Renderer& renderer, common::GameInput& input) : selectAction{input.addAction("ui_select")} {
    offsetStack.emplace_back(0,0);

    uiRoot = std::make_shared<ui::UIRootNode>();
    themeManager.selfRegister();
    defaultTheme = common::Assets::LoadVirtual("phenyl/themes/default", ui::Theme{util::parseJson(EMBED_DEFAULT_THEME_JSON)});
    setCurrentTheme(defaultTheme);

    input.addActionBinding("ui_select", "mouse.button_left");
}

void UIManager::renderUI (Canvas& canvas) {
    uiRoot->render(canvas);
}

void UIManager::setMousePos (glm::vec2 _mousePos) {
    mousePos = _mousePos;
    uiRoot->setMousePos(mousePos);
}

bool UIManager::setMouseDown (bool _mouseDown) {
    if (mouseDown != _mouseDown) {
        mouseDown = _mouseDown;
        if (mouseDown) {
            return uiRoot->onMousePress();
        } else {
            uiRoot->onMouseRelease();
        }
    }
    return false;
}

void UIManager::addUINode (const std::shared_ptr<ui::UIComponentNode>& uiNode, glm::vec2 pos) {
    uiRoot->addChildNode(uiNode, pos);
}

void UIManager::setCurrentTheme (common::Asset<ui::Theme> theme) {
    currentTheme = std::move(theme);
    uiRoot->applyTheme(currentTheme.get());
}

void UIManager::updateUI () {
    bool newMouse = selectAction.value();
    if (newMouse != mouseDown) {
        if (newMouse) {
            if (uiRoot->onMousePress()) {
                //uiInput.consumeProxyInput(selectAction);
                // TODO
            }
        } else {
            uiRoot->onMouseRelease();
        }
    }
    mouseDown = newMouse;
}

std::string_view UIManager::getName () const noexcept {
    return "UIManager";
}

UIManager::~UIManager () = default;

ui::Theme* UIThemeManager::load (std::istream& data, std::size_t id) {
    auto theme = ui::loadTheme(data);
    if (!theme) {
        return nullptr;
    }

    themes[id] = std::move(theme);
    return themes[id].get();
}

const char* UIThemeManager::getFileType () const {
    return ".json";
}

void UIThemeManager::queueUnload (std::size_t id) {
    if (onUnload(id)) {
        themes.remove(id);
    }
}

void UIThemeManager::selfRegister () {
    common::Assets::AddManager(this);
}

ui::Theme* UIThemeManager::load (ui::Theme&& obj, std::size_t id) {
    themes[id] = std::make_unique<ui::Theme>(std::move(obj));

    return themes[id].get();
}
