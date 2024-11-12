#include "graphics/ui/widgets/container.h"

#include "graphics/canvas/canvas.h"

using namespace phenyl::graphics;

ContainerWidget::ContainerWidget (std::unique_ptr<Widget> childWidget, const Modifier& modifier) : Widget{modifier}, childWidget{std::move(childWidget)} {
    if (childWidget) {
        childWidget->setParent(this);
    }
}

Widget* ContainerWidget::replace (std::unique_ptr<Widget> newChild) {
    auto* ptr = newChild.get();
    childWidget = std::move(newChild);
    childWidget->setParent(this);

    return ptr;
}

void ContainerWidget::setBgColor (glm::vec4 newBgColor) {
    widgetBgColour = newBgColor;
}

void ContainerWidget::setBorderColor (glm::vec4 newBorderColor) {
    widgetBorderColour = newBorderColor;
}

void ContainerWidget::setBorderSize (float newBorderSize) {
    widgetBorderSize = newBorderSize;
}

void ContainerWidget::setOffset (glm::vec2 newOffset) {
    Widget::setOffset(newOffset);

    if (childWidget) {
        childWidget->setOffset(offset() + glm::vec2{borderSize() + childWidget->modifier().padding, borderSize() + childWidget->modifier().padding});
    }
}

void ContainerWidget::update () {
    if (childWidget) {
        childWidget->update();
    }
}

void ContainerWidget::measure (const WidgetConstraints& constraints) {
    glm::vec2 minSize = modifier().minSize;
    glm::vec2 maxSize = {modifier().maxWidth ? std::min(*modifier().maxWidth, constraints.maxSize.x) : constraints.maxSize.x, modifier().maxHeight ? std::min(*modifier().maxHeight, constraints.maxSize.y) : constraints.maxSize.y};

    if (childWidget) {
        childWidget->measure(WidgetConstraints{
            .minWidth = std::max(0.0f, minSize.x - 2 * borderSize() - 2 * childWidget->modifier().padding),
            .minHeight = std::max(0.0f, minSize.y - 2 * borderSize()  - 2 * childWidget->modifier().padding),
            .maxSize = glm::max(glm::vec2{0.0f, 0.0f}, maxSize - 2.0f * glm::vec2{borderSize(), borderSize()} - 2.0f * glm::vec2{childWidget->modifier().padding, childWidget->modifier().padding})
        });

        childWidget->setOffset(offset() + glm::vec2{borderSize() + childWidget->modifier().padding, borderSize() + childWidget->modifier().padding});
        setDimensions(glm::max(minSize, childWidget->dimensions() + 2.0f * glm::vec2(borderSize(), borderSize()) + 2.0f * glm::vec2(childWidget->modifier().padding, childWidget->modifier().padding)));
    } else {
        setDimensions(minSize);
    }
}

void ContainerWidget::render (Canvas& canvas) {
    if (dimensions().x >= 2 * borderSize() && dimensions().y >= 2 * borderSize()) {
        canvas.render({borderSize(), borderSize()}, CanvasRect{
            .size = dimensions() - glm::vec2{borderSize(), borderSize()}
        }, CanvasStyle{
            .fill = CanvasFill::FILLED,
            .colour = bgColor()
        });

        canvas.render({0.0f, 0.0f}, CanvasRect{
            .size = dimensions()
        }, CanvasStyle{
            .fill = CanvasFill::OUTLINE,
            .outlineSize = borderSize(),
            .colour = borderColor(),
        });
    }

    if (childWidget) {
        canvas.pushOffset({borderSize() + childWidget->modifier().padding, borderSize() + childWidget->modifier().padding});
        childWidget->render(canvas);
        canvas.popOffset();
    }
}

bool ContainerWidget::pointerUpdate (glm::vec2 pointer) {
    if (childWidget) {
        childWidget->pointerUpdate(pointer - glm::vec2{borderSize() + childWidget->modifier().padding, borderSize() + childWidget->modifier().padding});
    }
    return Widget::pointerUpdate(pointer);
}

void ContainerWidget::pointerLeave () {
    if (childWidget) {
        childWidget->pointerLeave();
    }
    Widget::pointerLeave();
}
