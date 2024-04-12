#pragma once

#include <memory>
#include <utility>

//#include "graphics/ui/ui_manager.h"
#include "ui_anchor.h"
#include "graphics/ui/themes/theme_properties.h"

namespace phenyl::graphics {
    class Canvas;
    class UIManager;
}

namespace phenyl::graphics::ui {
    class UIContainerNode;
    class UIComponentNode {
    private:
        UIContainerNode* parent = nullptr;
        glm::vec2 mousePos{};
        ThemeProperties themeProperties;
        Theme* theme = nullptr;

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

        [[nodiscard]] Theme* getThemePtr () const {
            return theme;
        }

        virtual void onThemeUpdate () {

        }

        virtual void onDestroyRequest () {

        }

        void clearDirty ();
        void markDirty ();

    public:
        explicit UIComponentNode (const std::string& themeClass, const std::string& fallbackClass = "default", const std::string& classPrefix = "") : themeProperties(themeClass, fallbackClass, classPrefix) {}
        virtual ~UIComponentNode() = default;
        virtual void render (Canvas& canvas) = 0;
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

        void applyTheme (Theme* _theme) {
            this->theme = _theme;
            themeProperties.applyTheme(theme);
            onThemeUpdate();
        }

        void queueDestroy ();

        virtual bool isDirty ();


        friend class UIContainerNode;

    };
}