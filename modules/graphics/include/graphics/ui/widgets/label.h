#pragma once

#include <string>

#include "common/assets/assets.h"
#include "graphics/maths_headers.h"

#include "widget.h"


namespace phenyl::graphics {
    class Font;

    class LabelWidget : public Widget {
    private:
        std::string labelText;
        common::Asset<Font> labelFont;
        unsigned int labelTextSize = 12;
        glm::vec4 labelColour = {1.0f, 1.0f, 1.0f, 1.0f};

        void updateModifier ();
    public:
        explicit LabelWidget (std::string text = "", unsigned int textSize = 12, common::Asset<Font> font = {});
        ~LabelWidget () override;

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

        [[nodiscard]] const common::Asset<Font>& font () const noexcept {
            return labelFont;
        }
        void setFont (common::Asset<Font> newFont);
    };
}
