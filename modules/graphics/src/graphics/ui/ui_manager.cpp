#include "graphics/ui/ui_manager.h"
#include "graphics/renderers/renderer.h"
#include "graphics/renderlayer/ui_layer.h"
#include "graphics/ui/nodes/ui_root.h"
#include "graphics/ui/themes/theme.h"
#include "graphics/ui/themes/theme_class.h"
#include "common/assets/assets.h"

#include "resources/themes/default_theme.json.h"

#include "logging/logging.h"

using namespace phenyl::graphics;

static phenyl::Logger LOGGER{"UI_MANAGER", detail::GRAPHICS_LOGGER};

static void addCurve (std::vector<glm::vec2>& points, glm::vec2 start, glm::vec2 end, glm::vec2 centre, unsigned int quanta) {
    auto d1 = start - centre;
    auto d2 = end - centre;

    for (int i = 0; i < quanta + 2; i++) {
        auto theta = static_cast<float>(i) / static_cast<float>(quanta + 1) * glm::pi<float>() / 2;
        points.emplace_back(centre + d1 * glm::cos(theta) + d2 * glm::sin(theta));
    }
}

UIManager::UIManager (Renderer& renderer) : glyphAtlas{renderer} {
    uiLayer = &renderer.addLayer<UIRenderLayer>(glyphAtlas);
    fontManager = std::make_unique<FontManager>(renderer.getViewport(), glyphAtlas, *uiLayer);
    fontManager->selfRegister();

    offsetStack.emplace_back(0,0);

    uiRoot = std::make_shared<ui::UIRootNode>();
    themeManager.selfRegister();
    defaultTheme = common::Assets::LoadVirtual("phenyl/themes/default", ui::Theme{util::parseJson(EMBED_DEFAULT_THEME_JSON)});
    setCurrentTheme(defaultTheme);

    addProxyInputSources(renderer.getViewport().getProxySources());
    setupInputActions();
}

/*void UIManager::renderText (common::Asset<Font> font, const std::string& text, int size, int x, int y) {
    renderText(std::move(font), text, size, x, y, {1.0f, 1.0f, 1.0f});
}*/

/*void UIManager::renderText (common::Asset<Font> font, const std::string& text, int size, int x, int y,
                            glm::vec3 colour) {
    auto offVec = offsetStack.back() + glm::vec2{x, y};
    textBuf.emplace_back(offVec, font->renderText(text, size, 0, 0, colour));
}*/

void UIManager::renderUI () {
    uiLayer->setScreenSize(screenSize);

    uiRoot->render(*this);
    uiLayer->uploadData();
}

void UIManager::renderRect (glm::vec2 topLeftPos, glm::vec2 size, glm::vec4 bgColour, glm::vec4 borderColour, float borderSize) {
    auto screenPos = topLeftPos + offsetStack.back();
    glm::vec2 vertices[] {
            screenPos, glm::vec2{screenPos.x + size.x, screenPos.y}, screenPos + size, glm::vec2{screenPos.x, screenPos.y + size.y}
    };
    uiLayer->renderConvexPolyAA(vertices, bgColour);
    uiLayer->renderPolyLineAA(vertices, borderColour, borderSize, true);
}

void UIManager::renderRoundedRect(glm::vec2 topLeft, glm::vec2 size, glm::vec4 colour, float cornerRadius, unsigned int quanta) {
    auto screenPos = topLeft + offsetStack.back();

    std::vector<glm::vec2> vertices;
    addCurve(vertices, screenPos + glm::vec2{0, cornerRadius}, screenPos + glm::vec2{cornerRadius, 0}, screenPos + glm::vec2{cornerRadius, cornerRadius}, quanta);
    addCurve(vertices, screenPos + glm::vec2{size.x - cornerRadius, 0.0f}, screenPos + glm::vec2{size.x, cornerRadius}, screenPos + glm::vec2{size.x - cornerRadius, cornerRadius}, quanta);
    addCurve(vertices, screenPos + glm::vec2{size.x, size.y - cornerRadius}, screenPos + glm::vec2{size.x - cornerRadius, size.y}, screenPos + glm::vec2{size.x - cornerRadius, size.y - cornerRadius}, quanta);
    addCurve(vertices, screenPos + glm::vec2{cornerRadius, size.y}, screenPos + glm::vec2{0.0f, size.y - cornerRadius}, screenPos + glm::vec2{cornerRadius, size.y - cornerRadius}, quanta);

    uiLayer->renderConvexPolyAA(vertices, colour);
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

void UIManager::pushOffset (glm::vec2 relOffset) {
    offsetStack.push_back(offsetStack[offsetStack.size() - 1] + relOffset);
}

void UIManager::popOffset () {
    if (offsetStack.size() == 1) {
        PHENYL_LOGE(LOGGER, "Attempted to pop too many elements off offset stack!");
    } else {
        offsetStack.pop_back();
    }
}

void UIManager::addUINode (const std::shared_ptr<ui::UIComponentNode>& uiNode, glm::vec2 pos) {
    uiRoot->addChildNode(uiNode, pos);
}

void UIManager::renderText (phenyl::common::Asset<Font>& font, std::uint32_t size, const std::string& text,
                            glm::vec2 pos) {
    renderText(font, size, text,  pos, glm::vec3{1.0f, 1.0f, 1.0f});
}

void UIManager::renderText (phenyl::common::Asset<Font>& font, std::uint32_t size, const std::string& text, glm::vec2 pos,
                            glm::vec3 colour) {
    font->renderText(size, text, offsetStack.back() + pos,  colour);
}

void UIManager::setCurrentTheme (common::Asset<ui::Theme> theme) {
    currentTheme = std::move(theme);
    uiRoot->applyTheme(currentTheme.get());
}

void UIManager::addProxyInputSources (const std::vector<std::shared_ptr<common::ProxySource>>& proxySources) {
    for (const auto& i : proxySources) {
        uiInput.addInputSource(i);
    }
}

void UIManager::setupInputActions () {
    selectAction = uiInput.addInputMapping("ui_select", "mouse_left");
}

void UIManager::updateUI () {
    bool newMouse = uiInput.isActive(selectAction);
    if (newMouse != mouseDown) {
        if (newMouse) {
            if (uiRoot->onMousePress()) {
                uiInput.consumeProxyInput(selectAction);
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
