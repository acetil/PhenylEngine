#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <vector>

#include "graphics/font/font_manager.h"
#include "graphics/font/font.h"
#include "graphics/maths_headers.h"
#include "graphics/ui/components/ui_component.h"

namespace graphics {
//#ifndef FONT_H
class Font;
//#endif
    //#ifndef RENDERER_H
    class Renderer;
    //#endif

    //#ifndef UI_LAYER_H
    class UIRenderLayer;
    //#endif

    //#ifndef GRAPHICS_H
    namespace detail {
        class Graphics;
    }
    //#endif

    class RenderedText;
    namespace ui {
        class UIRootNode;
    }

    class UIManager {
    private:
        FontManager fontManager;
        std::unordered_map<std::string, Font> fonts;
        std::shared_ptr<UIRenderLayer> uiLayer;
        std::vector<std::pair<glm::vec2, RenderedText>> textBuf;
        std::vector<std::pair<glm::vec2, RenderedText&>> textBuf2;
        glm::vec2 screenSize = {800, 600};
        glm::vec2 mousePos = {0, 0};
        bool mouseDown = false;
        std::vector<glm::vec2> offsetStack;
        std::shared_ptr<ui::UIRootNode> uiRoot;
    public:
        UIManager(Renderer* renderer, FontManager& _fontManager);
        void renderText(const std::string& font, const std::string& text, int size, int x, int y);
        //void renderText(const std::string& font, const std::string& text, int size, int x, int y, glm::vec3 colour);
        void renderText (const std::string& font, const std::string& text, int size, int x, int y, glm::vec3 colour);
        void renderText (RenderedText& text, int x, int y);
        RenderedText getRenderedText (const std::string& font, const std::string& text, int size, glm::vec3 colour);
        void renderRect (glm::vec2 topLeftPos, glm::vec2 size, glm::vec4 bgColour, glm::vec4 borderColour, float cornerRadius = 0.0f, float borderSize = 0.0f);
        void renderUI ();
        void addRenderLayer (const std::shared_ptr<detail::Graphics>& graphics, Renderer* renderer);
        void setMousePos (glm::vec2 _mousePos);
        bool setMouseDown (bool mouseDown);

        void addUINode (const std::shared_ptr<ui::UIComponentNode>& uiNode, glm::vec2 pos);

        void pushOffset (glm::vec2 relOffset);
        void popOffset ();
    };
}
