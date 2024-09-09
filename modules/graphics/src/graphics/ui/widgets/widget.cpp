#include "graphics/ui/widgets/widget.h"
#include "logging/logging.h"

using namespace phenyl::graphics;

Widget::Widget (const Modifier& modifier) : widgetModifier{modifier} {}
Widget::~Widget () = default;

void Widget::measure (const WidgetConstraints& constraints) {
    float maxWidth = modifier().maxWidth ? std::min(*modifier().maxWidth, constraints.maxSize.x) : constraints.maxSize.x;
    float maxHeight = modifier().maxHeight ? std::min(*modifier().maxHeight, constraints.maxSize.y) : constraints.maxSize.y;
    setDimensions(glm::vec2{maxWidth, maxHeight});
}

void Widget::update () {}

void Widget::queueDestroy () {
    PHENYL_ASSERT_MSG(parentWidget, "Attempted to queue destruction on unparented widget!");
    parentWidget->queueChildDestroy(this);
}

void Widget::setParent (Widget* parent) {
    parentWidget = parent;
}

void Widget::queueChildDestroy (Widget* child) {}

void Widget::setModifier (const Modifier& modifier) {
    widgetModifier = modifier;
}

void Widget::setDimensions (glm::vec2 newDims) {
    widgetSize = newDims;
}
