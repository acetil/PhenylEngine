#pragma once

#include "ui_node.h"

namespace phenyl::graphics::ui {
    class UIContainerNode : public UIComponentNode {
    private:
        std::size_t lockNum = 0;
        std::vector<UIComponentNode*> destroyQueue;
    protected:
        void addChild (const std::shared_ptr<UIComponentNode>& childNode) {
            childNode->setParent(this);
        }

        virtual void destroyChild (UIComponentNode* childNode) = 0;

        void lockChildDestruction ();
        void unlockChildDestruction ();

    public:
        explicit UIContainerNode (const std::string& themeClass, const std::string& fallbackClass = "default", const std::string& classPrefix = "") : UIComponentNode(themeClass, fallbackClass, classPrefix) {}

        void queueChildDestroy (UIComponentNode* childNode);

        friend class UIComponentNode;
    };
}