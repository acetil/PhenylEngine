#pragma once

#include <memory>
#include <utility>

//#include "graphics/ui/ui_manager.h"
#include "ui_anchor.h"
#include "graphics/ui/themes/theme_properties.h"

namespace graphics {
    class UIManager;
}

namespace graphics::ui {
    class UIComponentNode {
    private:
        std::weak_ptr<UIComponentNode> parent{};
        glm::vec2 mousePos{};
        ThemeProperties themeProperties;
    protected:
        std::shared_ptr<UIComponentNode> getParent () {
            return parent.lock();
        }

        glm::vec2 size{};
        glm::vec2 getMousePos () {
            return mousePos;
        }

        [[nodiscard]] const ThemeProperties& getTheme () const {
            return themeProperties;
        };

        virtual void onThemeUpdate (Theme* theme) {

        }

    public:
        explicit UIComponentNode (const std::string& themeClass, const std::string& fallbackClass = "default", const std::string& classPrefix = "") : themeProperties(themeClass, fallbackClass, classPrefix) {}
        virtual ~UIComponentNode() = default;
        virtual void render (UIManager& uiManager) = 0;
        virtual UIAnchor getAnchor () = 0;
        virtual void setSize (glm::vec2 _size) {
            size = _size;
        }

        void setMousePos (glm::vec2 _mousePos) {
            auto oldPos = mousePos;
            mousePos = _mousePos;
            onMousePosChange(oldPos);
        }

        virtual void onMousePosChange (glm::vec2 oldMousePos) {

        }

        virtual bool onMousePress () {
            return false;
        }

        virtual void onMouseRelease () {

        }

        void applyTheme (Theme* theme) {
            themeProperties.applyTheme(theme);
            onThemeUpdate(theme);
        }
    };
}