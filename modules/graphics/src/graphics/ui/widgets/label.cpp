#include "graphics/ui/widgets/label.h"

#include "graphics/canvas/canvas.h"
#include "graphics/detail/loggers.h"
#include "graphics/font/font.h"

using namespace phenyl::graphics;

static phenyl::Logger LOGGER{"LABEL", detail::GRAPHICS_LOGGER};

LabelWidget::LabelWidget (std::string text, unsigned int textSize, core::Asset<Font> font, const Modifier& modifier) : Widget{modifier}, labelText{std::move(text)}, labelTextSize{textSize} {
    labelFont = std::move(font);
    updateModifier();
}
// LabelWidget::~LabelWidget () = default;

void LabelWidget::render (Canvas& canvas) {
    if (!labelFont) {
        PHENYL_LOGD(LOGGER, "Ignoring label render due to missing font");
        return;
    }
    canvas.renderText({0.0f, 0.0f}, labelFont, textSize(), text(), colour());
}

void LabelWidget::setText (std::string newText) {
    labelText = std::move(newText);
    updateModifier();
}

void LabelWidget::setTextSize (unsigned int newTextSize) {
    labelTextSize = newTextSize;
    updateModifier();
}

void LabelWidget::setTextColour (glm::vec4 newColour) {
    labelColour = newColour;
}

void LabelWidget::setFont (core::Asset<Font> newFont) {
    labelFont = std::move(newFont);
    updateModifier();
}

void LabelWidget::updateModifier () {
    if (font()) {
        auto bounds = font()->getBounds(textSize(), text());
        setModifier(modifier().withSize(bounds.size));
    } else {
        setModifier(modifier().withSize({0, 0}));
    }
}
