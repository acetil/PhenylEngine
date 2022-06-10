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
    class UIContainerNode;
    class UIComponentNode {
    private:
        UIContainerNode* parent = nullptr;
        glm::vec2 mousePos{};
        ThemeProperties themeProperties;

        bool dirty = false;

        void setParent (UIContainerNode* parent);
    protected:
        UIContainerNode* getParent () {
            return parent;
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

        virtual void onDestroyRequest () {

        }

        void clearDirty ();
        void markDirty ();

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

        void queueDestroy ();

        virtual bool isDirty ();


        friend class UIContainerNode;

    };
}