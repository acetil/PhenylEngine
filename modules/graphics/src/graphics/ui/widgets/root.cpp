#include "graphics/ui/widgets/root.h"

#include "graphics/canvas/canvas.h"

using namespace phenyl::graphics;

RootWidget::RootWidget () = default;

Widget* RootWidget::insert (std::unique_ptr<Widget> widget) {
    auto* ptr = widget.get();
    children.emplace_back(std::move(widget));
    ptr->setParent(this);
    ptr->setOffset({0, 0});
    return ptr;
}

Widget* RootWidget::pick (glm::vec2 pointer) noexcept {
    for (auto it = children.rbegin(); it != children.rend(); ++it) {
        if (auto* ptr = (*it)->pick(pointer - (*it)->modifier().offset)) {
            return ptr;
        }
    }
    return nullptr;
}

void RootWidget::measure (const WidgetConstraints& constraints) {
    for (auto& i : children) {
        i->measure(WidgetConstraints{
            .maxSize = constraints.maxSize - i->modifier().offset
        });
    }
}

void RootWidget::update () {
    std::erase_if(children, [this] (const auto& w) {
        return widgetsToDelete.contains(w.get());
    });
    widgetsToDelete.clear();

    for (auto& i : children) {
        i->update();
    }
}

void RootWidget::render (Canvas& canvas) {
    for (auto& i : children) {
        canvas.pushOffset(i->modifier().offset);
        i->render(canvas);
        canvas.popOffset();
    }
}

void RootWidget::queueChildDestroy (Widget* child) {
    widgetsToDelete.emplace(child);
}

bool RootWidget::pointerUpdate (glm::vec2 pointer) {
    auto it = children.rbegin();
    while (it != children.rend()) {
        bool childResult = (*it)->pointerUpdate(pointer - (*it)->modifier().offset);
        ++it;
        if (childResult) {
            break;
        }
    }

    for ( ; it != children.rend(); ++it) {
        (*it)->pointerLeave();
    }

    return Widget::pointerUpdate(pointer);
}

void RootWidget::pointerLeave () {
    // Should never occur
    for (auto& i : children) {
        i->pointerLeave();
    }
    Widget::pointerLeave();
}
