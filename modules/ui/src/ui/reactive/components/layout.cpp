#include "ui/reactive/components/layout.h"

using namespace phenyl::graphics;

namespace {
class UILayoutNode : public UINode {
public:
    UILayoutNode (const UILayoutProps& props) :
        UINode{props.modifier},
        m_axis{props.axis},
        m_arrangement{props.arrangement},
        m_alignment{props.alignment} {}

    void measure (const WidgetConstraints& constraints) override {
        glm::vec2 constraintDims = glm::vec2{
          modifier().maxWidth ? glm::min(*modifier().maxWidth, constraints.maxSize.x) : constraints.maxSize.x,
          modifier().maxHeight ? glm::min(*modifier().maxHeight, constraints.maxSize.y) : constraints.maxSize.y};

        auto dims = glm::max(modifier().minSize, constraintDims);

        glm::vec2 mainAxis{};
        glm::vec2 secondAxis{};
        glm::vec2 currOff{};

        switch (m_axis) {
        case LayoutAxis::LEFT:
        case LayoutAxis::RIGHT:
            mainAxis = {1, 0};
            secondAxis = {0, 1};
            break;
        case LayoutAxis::UP:
        case LayoutAxis::DOWN:
            mainAxis = {0, 1};
            secondAxis = {1, 0};
            break;
        }

        float secondLength = glm::dot(dims, secondAxis);
        float remainingLength = measureChildren(dims, glm::abs(mainAxis), glm::abs(secondAxis));

        float spacing = 0.0f;
        if (m_arrangement == LayoutArrangement::CENTER) {
            currOff += mainAxis * (remainingLength / 2);
        } else if (m_arrangement == LayoutArrangement::END) {
            currOff += mainAxis * remainingLength;
        } else if (m_arrangement == LayoutArrangement::SPACED) {
            spacing = m_children.size() > 1 ? remainingLength / ((float) m_children.size() - 1) : 0.0f;
        }

        m_childOffsets.clear();
        for (auto& i : m_children) {
            auto childDims = i->dimensions();
            float remainingSecond = std::max(0.0f,
                secondLength - glm::dot(childDims, secondAxis) - i->modifier().padding * 2 -
                    glm::dot(secondAxis, i->modifier().offset));
            switch (m_alignment) {
            case LayoutAlignment::START:
                m_childOffsets.emplace_back(currOff + secondAxis * i->modifier().padding + i->modifier().offset);
                break;
            case LayoutAlignment::END:
                m_childOffsets.emplace_back(
                    currOff + secondAxis * (remainingSecond + i->modifier().padding) + i->modifier().offset);
                break;
            case LayoutAlignment::CENTER:
                m_childOffsets.emplace_back(
                    currOff + secondAxis * (remainingSecond / 2 + i->modifier().padding) + i->modifier().offset);
                break;
            }

            currOff +=
                mainAxis * (glm::dot(mainAxis, childDims + i->modifier().offset) + i->modifier().padding * 2 + spacing);
        }

        if (m_axis == LayoutAxis::UP || m_axis == LayoutAxis::LEFT) {
            // Flip offsets
            float totalLength = glm::dot(mainAxis, dims);
            for (std::size_t i = 0; i < m_children.size(); i++) {
                m_childOffsets[i] = secondAxis * glm::dot(m_childOffsets[i], secondAxis) +
                    mainAxis * (totalLength - glm::dot(m_childOffsets[i] + m_children[i]->dimensions(), mainAxis));
            }
        }

        setDimensions(dims);
    }

    void render (Canvas& canvas) override {
        PHENYL_DASSERT(m_children.size() == m_childOffsets.size());

        auto childIt = m_children.begin();
        auto offIt = m_childOffsets.begin();
        while (childIt != m_children.end()) {
            canvas.pushOffset(*offIt);
            (*childIt)->render(canvas);
            canvas.popOffset();

            ++childIt;
            ++offIt;
        }
    }

    void addChild (std::unique_ptr<UINode> node) override {
        node->setParent(this);
        m_children.emplace_back(std::move(node));
    }

    UINode* pick (glm::vec2 pointer) noexcept override {
        if (pointer.x < 0.0f || pointer.x >= dimensions().x || pointer.y < 0.0f || pointer.y >= dimensions().y) {
            return nullptr;
        }

        PHENYL_DASSERT(m_children.size() == m_childOffsets.size());
        for (std::size_t i = 0; i < m_children.size(); i++) {
            if (auto* ptr = m_children[i]->pick(pointer - m_childOffsets[i])) {
                return ptr;
            }
        }

        return this;
    }

    bool doPointerUpdate (glm::vec2 pointer) override {
        assert(m_children.size() == m_childOffsets.size());

        auto it = m_children.rbegin();
        auto offIt = m_childOffsets.rbegin();
        while (it != m_children.rend()) {
            bool childResult = (*it)->doPointerUpdate(pointer - *offIt);
            ++it;
            ++offIt;
            if (childResult) {
                break;
            }
        }

        for (; it != m_children.rend(); ++it) {
            (*it)->onPointerLeave();
            ++offIt;
        }

        return UINode::doPointerUpdate(pointer);
    }

    void onPointerLeave () override {
        for (auto& i : m_children) {
            i->onPointerLeave();
        }
        UINode::onPointerLeave();
    }

private:
    LayoutAxis m_axis;
    LayoutArrangement m_arrangement;
    LayoutAlignment m_alignment;

    std::vector<std::unique_ptr<UINode>> m_children;
    std::vector<glm::vec2> m_childOffsets;

    float measureChildren (glm::vec2 dims, glm::vec2 mainAxis, glm::vec2 secondAxis) {
        float remainingLength = glm::dot(mainAxis, dims);
        glm::vec2 secondDim = secondAxis * glm::dot(secondAxis, dims);
        float totalWeight = 0.0f;

        for (auto& i : m_children) {
            if (i->modifier().weight > 0.0f) {
                totalWeight += i->modifier().weight;
            } else {
                i->measure(WidgetConstraints{
                  .maxSize = (remainingLength - 2 * i->modifier().padding) * mainAxis +
                      (secondDim - 2 * i->modifier().padding * secondAxis) - i->modifier().offset,
                });

                remainingLength = std::max(0.0f,
                    remainingLength - glm::dot(mainAxis, i->dimensions()) - 2 * i->modifier().padding -
                        glm::dot(mainAxis, i->modifier().offset));
            }
        }

        if (totalWeight == 0.0f) {
            return std::max(0.0f, remainingLength);
        }

        for (auto& i : m_children) {
            if (i->modifier().weight > 0.0f) {
                i->measure(WidgetConstraints{
                  .maxSize =
                      (remainingLength * i->modifier().weight / totalWeight - 2 * i->modifier().padding) * mainAxis +
                      (secondDim - 2 * i->modifier().padding * secondAxis) - i->modifier().offset});
                totalWeight -= i->modifier().weight;

                remainingLength = std::max(0.0f,
                    remainingLength - glm::dot(mainAxis, i->dimensions()) - 2 * i->modifier().padding -
                        glm::dot(mainAxis, i->modifier().offset));
            }
        }

        return std::max(0.0f, remainingLength);
    }
};
} // namespace

UILayoutComponent::UILayoutComponent (UI& ui, UILayoutProps props) : UIComponent{ui, std::move(props)} {}

void UILayoutComponent::render (UI& ui) const {
    ui.constructNode<UILayoutNode>(props());
    props().children(ui);
}

UIColumnComponent::UIColumnComponent (UI& ui, UIColumnProps props) : UIComponent{ui, std::move(props)} {}

void UIColumnComponent::render (UI& ui) const {
    ui.render<UILayoutComponent>(UILayoutProps{
      .axis = axis(),
      .arrangement = props().arrangement,
      .alignment = props().alignment,
      .modifier = props().modifier,
      .children = [this] (UI& ui) { props().children(ui); },
    });
}

LayoutAxis UIColumnComponent::axis () const noexcept {
    switch (props().direction) {
    case ColumnDirection::UP:
        return LayoutAxis::UP;
    case ColumnDirection::DOWN:
        return LayoutAxis::DOWN;
    }

    PHENYL_ABORT("Unexpected value for ColumnDirection: {}", static_cast<std::uint32_t>(props().direction));
}

UIRowComponent::UIRowComponent (UI& ui, UIRowProps props) : UIComponent{ui, std::move(props)} {}

void UIRowComponent::render (UI& ui) const {
    ui.render<UILayoutComponent>(UILayoutProps{
      .axis = axis(),
      .arrangement = props().arrangement,
      .alignment = props().alignment,
      .modifier = props().modifier,
      .children = [this] (UI& ui) { props().children(ui); },
    });
}

LayoutAxis UIRowComponent::axis () const noexcept {
    switch (props().direction) {
    case RowDirection::LEFT:
        return LayoutAxis::LEFT;
    case RowDirection::RIGHT:
        return LayoutAxis::RIGHT;
    }

    PHENYL_ABORT("Unexpected value for RowDirection: {}", static_cast<std::uint32_t>(props().direction));
}
