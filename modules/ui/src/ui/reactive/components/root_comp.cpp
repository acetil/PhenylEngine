#include "root_comp.h"

#include "graphics/detail/loggers.h"
#include "ui/reactive/render.h"

using namespace phenyl::graphics;

static phenyl::Logger LOGGER{"ROOT2", detail::GRAPHICS_LOGGER};

namespace {
class UIRootNode : public UINode {
public:
    UIRootNode () : UINode{{}} {
        PHENYL_TRACE(LOGGER, "Constructed UIRootNode");
    }

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
        node->setParent(this, m_children.size());
        m_children.emplace_back(std::move(node));
    }

    void replaceChild (std::size_t index, std::unique_ptr<UINode> node) override {
        PHENYL_DASSERT_MSG(index < m_children.size(),
            "Attempted to replace non-existent child (index: {}, numChildren: {})", index, m_children.size());
        node->setParent(this, index);
        m_children[index] = std::move(node);
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
            auto& node = *it;
            bool childResult = node->doPointerUpdate(pointer - node->modifier().offset);
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

UIRenderResult UIRootComponent::render (UIContext& ctx) const {
    auto builder = ctx.makeNode<UIRootNode>();
    for (const auto& child : *props().comps) {
        builder.withChild(child.comp(ctx, child.id));
    }
    return builder.build();
}
