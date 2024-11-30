#pragma once

#include <string>

#include "core/assets/assets.h"
#include "graphics/maths_headers.h"

#include "widget.h"


namespace phenyl::graphics {
    class Font;

    class LabelWidget : public Widget {
    private:
        std::string labelText;
        core::Asset<Font> labelFont;
        unsigned int labelTextSize = 12;
        glm::vec4 labelColour = {1.0f, 1.0f, 1.0f, 1.0f};

        void updateModifier ();
    public:
        explicit LabelWidget (std::string text = "", unsigned int textSize = 12, core::Asset<Font> font = {}, const Modifier& modifier = {});
        // LabelWidget (const LabelWidget&) = delete;
        // LabelWidget (LabelWidget&&) = default;
        //
        // LabelWidget& operator= (const LabelWidget&) = delete;
        // LabelWidget& operator= (LabelWidget&&) = default;
        //
        // ~LabelWidget () override;

        void render (Canvas& canvas) override;

        [[nodiscard]] std::string_view text () const noexcept {
            return labelText;
        }
        void setText (std::string newText);

        [[nodiscard]] unsigned int textSize () const noexcept {
            return labelTextSize;
        }
        void setTextSize (unsigned int newTextSize);

        [[nodiscard]] glm::vec4 colour () const noexcept {
            return labelColour;
        }
        void setTextColour (glm::vec4 newColour);

        [[nodiscard]] const core::Asset<Font>& font () const noexcept {
            return labelFont;
        }
        void setFont (core::Asset<Font> newFont);
    };
}
