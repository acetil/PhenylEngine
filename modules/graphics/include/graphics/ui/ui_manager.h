#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <vector>

#include "graphics/font/font_manager.h"
#include "graphics/font/font.h"
#include "graphics/maths_headers.h"

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

    class UIManager {
    private:
        FontManager fontManager;
        std::unordered_map<std::string, Font> fonts;
        std::shared_ptr<UIRenderLayer> uiLayer;
        std::vector<RenderedText> textBuf;
        glm::vec2 screenSize = {800, 600};
    public:
        UIManager(Renderer* renderer, FontManager& _fontManager);
        void renderText(const std::string& font, const std::string& text, int size, int x, int y);
        //void renderText(const std::string& font, const std::string& text, int size, int x, int y, glm::vec3 colour);
        void renderText (const std::string& font, const std::string& text, int size, int x, int y, glm::vec3 colour);
        void renderRect (glm::vec2 topLeftPos, glm::vec2 size, glm::vec4 bgColour, glm::vec4 borderColour, float cornerRadius = 0.0f, float borderSize = 0.0f);
        void renderUI ();
        void addRenderLayer (const std::shared_ptr<detail::Graphics>& graphics, Renderer* renderer);
    };
}
