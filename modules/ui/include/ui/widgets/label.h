#pragma once

#include "core/assets/assets.h"
#include "graphics/maths_headers.h"
#include "widget.h"

#include <string>

namespace phenyl::graphics {
class Font;

class LabelWidget : public Widget {
public:
    explicit LabelWidget (std::string text = "", unsigned int textSize = 12, std::shared_ptr<Font> font = {},
        const Modifier& modifier = {});
    // LabelWidget (const LabelWidget&) = delete;
    // LabelWidget (LabelWidget&&) = default;
    //
    // LabelWidget& operator= (const LabelWidget&) = delete;
    // LabelWidget& operator= (LabelWidget&&) = default;
    //
    // ~LabelWidget () override;

    void render (Canvas& canvas) override;

    [[nodiscard]] std::string_view text () const noexcept {
        return m_text;
    }

    void setText (std::string newText);

    [[nodiscard]] unsigned int textSize () const noexcept {
        return m_textSize;
    }

    void setTextSize (unsigned int newTextSize);

    [[nodiscard]] glm::vec4 color () const noexcept {
        return m_color;
    }

    void setTextColor (glm::vec4 newColour);

    [[nodiscard]] const std::shared_ptr<Font>& font () const noexcept {
        return m_font;
    }

    void setFont (std::shared_ptr<Font> newFont);

private:
    std::string m_text;
    std::shared_ptr<Font> m_font;
    unsigned int m_textSize = 12;
    glm::vec4 m_color = {1.0f, 1.0f, 1.0f, 1.0f};

    void updateModifier ();
};
} // namespace phenyl::graphics
