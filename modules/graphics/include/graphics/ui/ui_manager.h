#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <vector>

#include "graphics/font/font_manager.h"
#include "graphics/font/font.h"
#include "graphics/maths_headers.h"
#include "graphics/ui/nodes/ui_node.h"
#include "graphics/ui/components/ui_component.h"
#include "graphics/ui/themes/forward.h"
#include "common/input/forward.h"
#include "common/input/remappable_proxy_input.h"
#include "common/assets/asset_manager.h"
#include "runtime/iresource.h"

namespace phenyl::graphics {
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

    class UIThemeManager : common::AssetManager<ui::Theme> {
    private:
        util::Map<std::size_t, std::unique_ptr<ui::Theme>> themes;
    public:
        ui::Theme* load(std::istream &data, std::size_t id) override;
        ui::Theme* load(phenyl::graphics::ui::Theme&& obj, std::size_t id) override;

        [[nodiscard]] const char* getFileType() const override;
        void queueUnload (std::size_t id) override;

        void selfRegister ();
    };

    class UIManager : public runtime::IResource {
    private:
        FontManager fontManager; // TODO
        common::Asset<Font> defaultFont;
        std::unordered_map<std::string, Font> fonts;
        std::shared_ptr<UIRenderLayer> uiLayer;
        std::vector<std::pair<glm::vec2, RenderedText>> textBuf;
        std::vector<std::pair<glm::vec2, RenderedText&>> textBuf2;
        glm::vec2 screenSize = {800, 600};
        glm::vec2 mousePos = {0, 0};
        bool mouseDown = false;
        std::vector<glm::vec2> offsetStack;
        std::shared_ptr<ui::UIRootNode> uiRoot;

        UIThemeManager themeManager;
        common::Asset<ui::Theme> defaultTheme;
        common::Asset<ui::Theme> currentTheme;

        //std::vector<std::shared_ptr<common::ProxySource>> inputSources;
        common::RemappableProxyInput uiInput;
        common::InputAction selectAction;
    public:
        UIManager(Renderer* renderer);
        ~UIManager();
        void renderText(common::Asset<Font> font, const std::string& text, int size, int x, int y);
        //void renderText(const std::string& font, const std::string& text, int size, int x, int y, glm::vec3 colour);
        void renderText (common::Asset<Font> font, const std::string& text, int size, int x, int y, glm::vec3 colour);
        void renderText (RenderedText text, int x, int y);
        void renderRect (glm::vec2 topLeftPos, glm::vec2 size, glm::vec4 bgColour, glm::vec4 borderColour, float cornerRadius = 0.0f, float borderSize = 0.0f);
        void renderUI ();
        void addRenderLayer (detail::Graphics& graphics, Renderer* renderer);
        void setMousePos (glm::vec2 _mousePos);
        bool setMouseDown (bool mouseDown);

        void addUINode (const std::shared_ptr<ui::UIComponentNode>& uiNode, glm::vec2 pos);
        template <typename T>
        void addUIComp (ui::UIComponent<T>& component, glm::vec2 pos) {
            addUINode(component.transferNode(), pos);
        }
        //void addTheme (const std::string& themePath);
        //void setCurrentTheme (const std::string& themeName);
        //void reloadCurrentTheme ();
        void setCurrentTheme (common::Asset<ui::Theme> theme);

        void pushOffset (glm::vec2 relOffset);
        void popOffset ();
        void addProxyInputSources (const std::vector<std::shared_ptr<common::ProxySource>>& proxySources);
        void setupInputActions ();
        void updateUI ();

        std::string_view getName() const noexcept override;
    };
}
