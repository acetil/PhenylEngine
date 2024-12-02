#pragma once

#include "graphics/maths_headers.h"
#include "core/iresource.h"

namespace phenyl::core {
    //void debugWorldRect (glm::vec2 topLeft, glm::vec2 bottomRight, glm::vec4 colour);
    void debugWorldRect (glm::vec2 topLeft, glm::vec2 bottomRight, glm::vec4 colour, glm::vec4 outlineColour=glm::vec4{});
    void debugWorldRectOutline (glm::vec2 topLeft, glm::vec2 bottomRight, glm::vec4 outlineColour);

    void debugWorldRect (glm::vec2 pos1, glm::vec2 pos2, glm::vec2 pos3, glm::vec2 pos4, glm::vec4 colour, glm::vec4 outlineColour=glm::vec4{});
    void debugWorldRectOutline (glm::vec2 pos1, glm::vec2 pos2, glm::vec2 pos3, glm::vec2 pos4, glm::vec4 outlineColour=glm::vec4{});

    void debugRect (glm::vec2 topLeft, glm::vec2 bottomRight, glm::vec4 colour, glm::vec4 outlineColour=glm::vec4{});
    void debugRectOutline (glm::vec2 topLeft, glm::vec2 bottomRight, glm::vec4 outlineColour);

    void debugRect (glm::vec2 pos1, glm::vec2 pos2, glm::vec2 pos3, glm::vec2 pos4, glm::vec4 colour, glm::vec4 outlineColour=glm::vec4{});
    void debugRectOutline (glm::vec2 pos1, glm::vec2 pos2, glm::vec2 pos3, glm::vec2 pos4, glm::vec4 outlineColour);

    void debugWorldLine (glm::vec2 start, glm::vec2 end, glm::vec4 colour);
    void debugLine (glm::vec2 start, glm::vec2 end, glm::vec4 colour);

    struct DebugRenderConfig : public IResource {
        bool doPhysicsRender = false;
        bool doProfileRender = true;

        [[nodiscard]] std::string_view getName() const noexcept override {
            return "DebugRuntimeConfig";
        }
    };
}

namespace phenyl::graphics {
    void debugWorldRect (glm::vec2 pos1, glm::vec2 pos2, glm::vec2 pos3, glm::vec2 pos4, glm::vec4 colour, glm::vec4 outlineColour);

    void debugScreenRect (glm::vec2 pos1, glm::vec2 pos2, glm::vec2 pos3, glm::vec2 pos4, glm::vec4 colour, glm::vec4 outlineColour);
    void debugWorldLine (glm::vec2 start, glm::vec2 end, glm::vec4 colour);
    void debugScreenLine (glm::vec2 start, glm::vec2 end, glm::vec4 colour);
}