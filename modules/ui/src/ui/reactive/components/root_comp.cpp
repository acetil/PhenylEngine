#include "root_comp.h"

using namespace phenyl::graphics;

namespace {
class UIRootNode : public UINode {
public:
    UIRootNode () : UINode{{}} {}

    void measure (const WidgetConstraints& constraints) override {
        for (auto& child : m_children) {
            child->measure({
              .maxSize = constraints.maxSize - child->modifier().offset,
            });
        }
    }

    void render (Canvas& canvas) override {
        for (auto& child : m_children) {
            canvas.pushOffset(child->modifier().offset);
            child->render(canvas);
            canvas.popOffset();
        }
    }

    void addChild (std::unique_ptr<UINode> node) override {
        node->setParent(this);
        m_children.emplace_back(std::move(node));
    }

    UINode* pick (glm::vec2 pointer) noexcept override {
        for (auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
            if (auto* ptr = (*it)->pick(pointer - (*it)->modifier().offset)) {
                return ptr;
            }
        }
        return nullptr;
    }

    bool doPointerUpdate (glm::vec2 pointer) override {
        auto it = m_children.rbegin();
        while (it != m_children.rend()) {
            bool childResult = (*it)->doPointerUpdate(pointer - (*it)->modifier().offset);
            ++it;
            if (childResult) {
                break;
            }
        }

        for (; it != m_children.rend(); ++it) {
            (*it)->onPointerLeave();
        }

        return UINode::doPointerUpdate(pointer);
    }

    void onPointerLeave () override {
        // Should never occur
        for (auto& i : m_children) {
            i->onPointerLeave();
        }
        UINode::onPointerLeave();
    }

private:
    std::vector<std::unique_ptr<UINode>> m_children;
};
} // namespace

UIRootComponent::UIRootComponent (UI& ui, UIRootProps&& rootProps) : UIComponent{ui, std::move(rootProps)} {
    useAtom(props().comps);
}

void UIRootComponent::render (UI& ui) const {
    ui.constructNode<UIRootNode>();

    for (const auto& child : *props().comps) {
        child.comp(ui, child.id);
    }
}
