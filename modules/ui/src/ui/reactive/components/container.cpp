#include "ui/reactive/components/container.h"

#include "ui/reactive/node.h"
#include "ui/reactive/ui.h"

using namespace phenyl::graphics;

namespace {
class UIContainerNode : public UINode {
public:
    UIContainerNode (const UIContainerProps& props) :
        UINode{props.modifier},
        m_bgColor{props.bgColor},
        m_borderColor{props.borderColor},
        m_borderSize{props.borderSize} {}

    void measure (const WidgetConstraints& constraints) override {
        glm::vec2 minSize = modifier().minSize;
        glm::vec2 maxSize = {//
          modifier().maxWidth ? std::min(*modifier().maxWidth, constraints.maxSize.x) : constraints.maxSize.x,
          modifier().maxHeight ? std::min(*modifier().maxHeight, constraints.maxSize.y) : constraints.maxSize.y};

        if (m_child) {
            m_child->measure(WidgetConstraints{
              .minWidth = std::max(0.0f, minSize.x - 2 * m_borderSize - 2 * m_child->modifier().padding),
              .minHeight = std::max(0.0f, minSize.y - 2 * m_borderSize - 2 * m_child->modifier().padding),
              .maxSize = glm::max(glm::vec2{0.0f, 0.0f},
                  maxSize - 2.0f * glm::vec2{m_borderSize, m_borderSize} -
                      2.0f * glm::vec2{m_child->modifier().padding, m_child->modifier().padding}),
            });

            setDimensions(glm::max(minSize,
                m_child->dimensions() + 2.0f * glm::vec2(m_borderSize, m_borderSize) +
                    2.0f * glm::vec2(m_child->modifier().padding, m_child->modifier().padding)));
        } else {
            setDimensions(minSize);
        }
    }

    void render (Canvas& canvas) override {
        if (dimensions().x >= 2 * m_borderSize && dimensions().y >= 2 * m_borderSize) {
            canvas.render({m_borderSize, m_borderSize},
                CanvasRect{
                  .size = dimensions() - glm::vec2{m_borderSize, m_borderSize},
                },
                CanvasStyle{
                  .fill = CanvasFill::FILLED,
                  .colour = m_bgColor,
                });

            canvas.render({0.0f, 0.0f},
                CanvasRect{
                  .size = dimensions(),
                },
                CanvasStyle{
                  .fill = CanvasFill::OUTLINE,
                  .outlineSize = m_borderSize,
                  .colour = m_borderColor,
                });
        }

        if (m_child) {
            canvas.pushOffset({m_borderSize + m_child->modifier().padding, m_borderSize + m_child->modifier().padding});
            m_child->render(canvas);
            canvas.popOffset();
        }
    }

    void addChild (std::unique_ptr<UINode> node) override {
        PHENYL_DASSERT_MSG(!m_child, "Attempted to create container with multiple components!");
        m_child = std::move(node);
        m_child->setParent(this);
    }

    UINode* pick (glm::vec2 pointer) noexcept override {
        if (pointer.x < 0.0f || pointer.x >= dimensions().x || pointer.y < 0.0f || pointer.y >= dimensions().y) {
            return nullptr;
        }

        auto* childResult = m_child ?
            m_child->pick(pointer -
                glm::vec2{m_borderSize + m_child->modifier().padding, m_borderSize + m_child->modifier().padding}) :
            nullptr;
        return childResult ? childResult : this;
    }

    bool doPointerUpdate (glm::vec2 pointer) override {
        if (m_child) {
            if (m_child->doPointerUpdate(pointer -
                    glm::vec2{m_borderSize + m_child->modifier().padding,
                      m_borderSize + m_child->modifier().padding})) {
                UINode::onPointerLeave();
                return true;
            }
        }
        return UINode::doPointerUpdate(pointer);
    }

    void onPointerLeave () override {
        if (m_child) {
            m_child->onPointerLeave();
        }
        UINode::onPointerLeave();
    }

private:
    std::unique_ptr<UINode> m_child;

    glm::vec4 m_bgColor;
    glm::vec4 m_borderColor;
    float m_borderSize;
};
} // namespace

UIContainerComponent::UIContainerComponent (UI& ui, UIContainerProps props) : UIComponent{ui, std::move(props)} {}

void UIContainerComponent::render (UI& ui) const {
    ui.constructNode<UIContainerNode>(props());
    props().child(ui);
}
