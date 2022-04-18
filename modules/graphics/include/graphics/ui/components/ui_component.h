#pragma once

#include <memory>
#include <utility>

//#include "graphics/ui/ui_manager.h"
#include "ui_anchor.h"

namespace graphics {
    class UIManager;
}

namespace graphics::ui {
    class UIComponentNode {
    private:
        std::weak_ptr<UIComponentNode> parent{};
        glm::vec2 mousePos;
    protected:
        std::shared_ptr<UIComponentNode> getParent () {
            return parent.lock();
        }

        glm::vec2 size{};
        glm::vec2 getMousePos () {
            return mousePos;
        }

    public:
        explicit UIComponentNode (std::weak_ptr<UIComponentNode> _parent) : parent{std::move(_parent)} {}
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
    };
}