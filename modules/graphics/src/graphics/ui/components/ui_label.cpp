#include "graphics/ui/components/ui_label.h"

using namespace graphics::ui;

const std::string& UILabel::getText () {
    // TODO: investigate why this doesn't work
    /*return getNodeOpt().thenMap([] (const std::shared_ptr<UILabelNode>& labelNode) {
        const auto& _text = labelNode->getCurrentText();
        return _text;
    }).orElse("");*/

    return getNodeOpt().then([] (const std::shared_ptr<UILabelNode>& labelNode) {
        const auto& _text = labelNode->getCurrentText();
        return util::Optional<const std::string&>{_text};
    }).orElse("");
}

void UILabel::setText (std::string&& newText) {
    getNodeOpt().ifPresent([&newText] (std::shared_ptr<UILabelNode>& labelNode) {
        labelNode->setText(std::forward<std::string>(newText));
    });
}
