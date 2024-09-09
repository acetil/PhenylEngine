#include "graphics/ui/widgets/root.h"

#include "graphics/canvas/canvas.h"

using namespace phenyl::graphics;

RootWidget::RootWidget () = default;

Widget* RootWidget::insert (std::unique_ptr<Widget> widget) {
    auto* ptr = widget.get();
    children.emplace_back(std::move(widget));
    ptr->setParent(this);
    return ptr;
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
