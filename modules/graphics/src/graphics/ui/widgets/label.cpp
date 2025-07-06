#include "graphics/ui/widgets/label.h"

#include "graphics/canvas/canvas.h"
#include "graphics/detail/loggers.h"
#include "graphics/font/font.h"

using namespace phenyl::graphics;

static phenyl::Logger LOGGER{"LABEL", detail::GRAPHICS_LOGGER};

LabelWidget::LabelWidget (std::string text, unsigned int textSize, std::shared_ptr<Font> font,
    const Modifier& modifier) :
    Widget{modifier},
    m_text{std::move(text)},
    m_textSize{textSize} {
    m_font = std::move(font);
    updateModifier();
}

// LabelWidget::~LabelWidget () = default;

void LabelWidget::render (Canvas& canvas) {
    if (!m_font) {
        PHENYL_LOGD(LOGGER, "Ignoring label render due to missing font");
        return;
    }
    canvas.renderText({0.0f, 0.0f}, m_font, textSize(), text(), color());
}

void LabelWidget::setText (std::string newText) {
    m_text = std::move(newText);
    updateModifier();
}

void LabelWidget::setTextSize (unsigned int newTextSize) {
    m_textSize = newTextSize;
    updateModifier();
}

void LabelWidget::setTextColor (glm::vec4 newColour) {
    m_color = newColour;
}

void LabelWidget::setFont (std::shared_ptr<Font> newFont) {
    m_font = std::move(newFont);
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
