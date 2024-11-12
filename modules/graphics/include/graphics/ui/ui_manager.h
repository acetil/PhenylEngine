#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <vector>

#include "graphics/maths_headers.h"
#include "graphics/font/font.h"
#include "graphics/font/glyph.h"
#include "graphics/font/glyph_atlas.h"
#include "graphics/ui/nodes/ui_node.h"
#include "graphics/ui/components/ui_component.h"
#include "graphics/ui/themes/forward.h"
#include "common/assets/asset_manager.h"
#include "common/input/axis_action.h"
#include "common/input/input_action.h"
#include "graphics/canvas/canvas.h"
#include "runtime/iresource.h"
#include "widgets/root.h"
#include "widgets/widget.h"

namespace phenyl::graphics {
    class FontManager;
    class Renderer;

    class CanvasRenderLayer;

    namespace ui {
        class UIRootNode;
    }

    class UIThemeManager : common::AssetManager<ui::Theme> {
    private:
        util::Map<std::size_t, std::unique_ptr<ui::Theme>> themes;
    public:
        ui::Theme* load(std::ifstream &data, std::size_t id) override;
        ui::Theme* load(phenyl::graphics::ui::Theme&& obj, std::size_t id) override;

        [[nodiscard]] const char* getFileType() const override;
        void queueUnload (std::size_t id) override;

        void selfRegister ();
    };

    class UIManager : public runtime::IResource {
    private:
        bool mouseDown = false;
        std::vector<glm::vec2> offsetStack;
        std::shared_ptr<ui::UIRootNode> uiRoot;
        std::unique_ptr<RootWidget> rootWidget;
        Widget* focusedWidget = nullptr;

        UIThemeManager themeManager;
        common::Asset<ui::Theme> defaultTheme;
        common::Asset<ui::Theme> currentTheme;
        common::InputAction selectAction;
        common::Axis2DInput mousePos;
    public:
        UIManager (Renderer& renderer, common::GameInput& input);
        ~UIManager() override;

        void renderUI (Canvas& canvas);

        void addUINode (const std::shared_ptr<ui::UIComponentNode>& uiNode, glm::vec2 pos);
        template <typename T>
        void addUIComp (ui::UIComponent<T>& component, glm::vec2 pos) {
            addUINode(component.transferNode(), pos);
        }
        //void addTheme (const std::string& themePath);
        //void setCurrentTheme (const std::string& themeName);
        //void reloadCurrentTheme ();
        void setCurrentTheme (common::Asset<ui::Theme> theme);
        void updateUI ();

        std::string_view getName() const noexcept override;

        [[nodiscard]] RootWidget& root () const noexcept {
            return *rootWidget;
        }
    };
}
