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
        m_children.emplace_back(std::move(node));
    }

private:
    std::vector<std::unique_ptr<UINode>> m_children;
};
} // namespace

UIRootComponent::UIRootComponent (UI& ui, UIRootProps&& rootProps) : UIComponent{ui, std::move(rootProps)} {
    useAtom(props().comps);
}

void UIRootComponent::render (UI& ui) {
    ui.constructNode<UIRootNode>();

    for (const auto& child : *props().comps) {
        child.comp(ui, child.id);
    }
}
