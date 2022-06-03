#include "graphics/ui/components/ui_flexbox.h"

#include "logging/logging.h"

using namespace graphics::ui;

void UIFlexbox::addUINode (std::shared_ptr<UIComponentNode> node, int index) {
    if (!node) {
        graphics::logging::log(LEVEL_ERROR, "Attempted to add already attached component to flexbox!");
        return;
    }

    getNodeOpt().ifPresent([&node, &index] (const std::shared_ptr<UIFlexBoxNode>& flexbox) {
        // TODO: move to flexbox
        if (index < 0) {
            flexbox->addComponent(node);
        } else {
            flexbox->addComponent(node, index);
        }
    });
}

void UIFlexbox::setAxes (Axis primaryAxis, Axis secondaryAxis) {
    // TODO: is this necessary?
    getNodeOpt().ifPresent([&primaryAxis, &secondaryAxis] (std::shared_ptr<UIFlexBoxNode>& flexBox) {
        flexBox->setAxes(primaryAxis, secondaryAxis);
    });
}
