#include "ui/reactive/components/label.h"

#include "graphics/detail/loggers.h"
#include "ui/reactive/node.h"
#include "ui/reactive/render.h"
#include "ui/reactive/ui.h"

using namespace phenyl::graphics;

static phenyl::Logger LOGGER{"LABEL2", detail::GRAPHICS_LOGGER};

namespace {
class UILabelNode : public UINode {
public:
    explicit UILabelNode (const UILabelProps& props) : UINode{GetModifier(props)}, m_props{props} {
        PHENYL_TRACE(LOGGER, "Created UILabelNode with text \"{}\"", props.text);
    }

    void render (Canvas& canvas) override {
        if (!m_props.font) {
            PHENYL_LOGD(LOGGER, "Ignoring label render due to missing font");
            return;
        }

        canvas.renderText({0.0f, 0.0f}, m_props.font, m_props.textSize, m_props.text, m_props.color);
    }

    void addChild (std::unique_ptr<UINode> node) override {
        PHENYL_ABORT("Attempted to add child to label node!");
    }

    void replaceChild (std::size_t index, std::unique_ptr<UINode> node) override {
        PHENYL_ABORT("Attempted to replace child in label node!");
    }

private:
    static Modifier GetModifier (const UILabelProps& props) {
        if (!props.font) {
            return props.modifier.withSize({0, 0});
        }

        auto bounds = props.font->getBounds(props.textSize, props.text);
        return props.modifier.withSize(bounds.size);
    }

    const UILabelProps& m_props;
};
} // namespace

UIRenderResult UILabelComponent::render (UIContext& ctx) const {
    return ctx.makeNode<UILabelNode>(props()).build();
}
