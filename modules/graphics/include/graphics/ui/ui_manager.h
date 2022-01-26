#pragma once

#include <unordered_map>
#include <string>
#include <memory>

#include "graphics/font/font_manager.h"
#include "graphics/buffer.h"
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
    class Graphics;
    //#endif

    class UIManager {
    private:
        FontManager fontManager;
        std::unordered_map<std::string, Font> fonts;
        std::shared_ptr<UIRenderLayer> uiLayer;
    public:
        UIManager(Renderer* renderer, FontManager& _fontManager);
        void renderText(const std::string& font, const std::string& text, int size, int x, int y);
        void renderText(const std::string& font, const std::string& text, int size, int x, int y, glm::vec3 colour);
        void addRenderLayer (const std::shared_ptr<Graphics>& graphics, Renderer* renderer);
    };
}
