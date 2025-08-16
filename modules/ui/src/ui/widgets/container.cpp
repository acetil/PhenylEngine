#include "ui/widgets/container.h"

#include "graphics/canvas/canvas.h"

using namespace phenyl::graphics;

ContainerWidget::ContainerWidget (std::unique_ptr<Widget> childWidget, const Modifier& modifier) :
    Widget{modifier},
    m_child{std::move(childWidget)} {
    if (childWidget) {
        childWidget->setParent(this);
    }
}

Widget* ContainerWidget::replace (std::unique_ptr<Widget> newChild) {
    auto* ptr = newChild.get();
    m_child = std::move(newChild);
    m_child->setParent(this);

    return ptr;
}

void ContainerWidget::setBgColor (glm::vec4 newBgColor) {
    m_bgColor = newBgColor;
}

void ContainerWidget::setBorderColor (glm::vec4 newBorderColor) {
    m_borderColor = newBorderColor;
}

void ContainerWidget::setBorderSize (float newBorderSize) {
    m_borderSize = newBorderSize;
}

void ContainerWidget::setOffset (glm::vec2 newOffset) {
    Widget::setOffset(newOffset);

    if (m_child) {
        m_child->setOffset(offset() +
            glm::vec2{borderSize() + m_child->modifier().padding, borderSize() + m_child->modifier().padding});
    }
}

void ContainerWidget::update () {
    if (m_child) {
        m_child->update();
    }
}

void ContainerWidget::measure (const WidgetConstraints& constraints) {
    glm::vec2 minSize = modifier().minSize;
    glm::vec2 maxSize = {modifier().maxWidth ? std::min(*modifier().maxWidth, constraints.maxSize.x) :
                                               constraints.maxSize.x,
      modifier().maxHeight ? std::min(*modifier().maxHeight, constraints.maxSize.y) : constraints.maxSize.y};

    if (m_child) {
        m_child->measure(WidgetConstraints{
          .minWidth = std::max(0.0f, minSize.x - 2 * borderSize() - 2 * m_child->modifier().padding),
          .minHeight = std::max(0.0f, minSize.y - 2 * borderSize() - 2 * m_child->modifier().padding),
          .maxSize = glm::max(glm::vec2{0.0f, 0.0f},
              maxSize - 2.0f * glm::vec2{borderSize(), borderSize()} -
                  2.0f * glm::vec2{m_child->modifier().padding, m_child->modifier().padding})});

        m_child->setOffset(offset() +
            glm::vec2{borderSize() + m_child->modifier().padding, borderSize() + m_child->modifier().padding});
        setDimensions(glm::max(minSize,
            m_child->dimensions() + 2.0f * glm::vec2(borderSize(), borderSize()) +
                2.0f * glm::vec2(m_child->modifier().padding, m_child->modifier().padding)));
    } else {
        setDimensions(minSize);
    }
}

void ContainerWidget::render (Canvas& canvas) {
    if (dimensions().x >= 2 * borderSize() && dimensions().y >= 2 * borderSize()) {
        canvas.render({borderSize(), borderSize()},
            CanvasRect{.size = dimensions() - glm::vec2{borderSize(), borderSize()}},
            CanvasStyle{.fill = CanvasFill::FILLED, .colour = bgColor()});

        canvas.render({0.0f, 0.0f}, CanvasRect{.size = dimensions()},
            CanvasStyle{
              .fill = CanvasFill::OUTLINE,
              .outlineSize = borderSize(),
              .colour = borderColor(),
            });
    }

    if (m_child) {
        canvas.pushOffset({borderSize() + m_child->modifier().padding, borderSize() + m_child->modifier().padding});
        m_child->render(canvas);
        canvas.popOffset();
    }
}

bool ContainerWidget::pointerUpdate (glm::vec2 pointer) {
    if (m_child) {
        m_child->pointerUpdate(pointer -
            glm::vec2{borderSize() + m_child->modifier().padding, borderSize() + m_child->modifier().padding});
    }
    return Widget::pointerUpdate(pointer);
}

void ContainerWidget::pointerLeave () {
    if (m_child) {
        m_child->pointerLeave();
    }
    Widget::pointerLeave();
}
