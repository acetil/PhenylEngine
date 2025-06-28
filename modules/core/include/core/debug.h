#pragma once

#include "core/iresource.h"
#include "graphics/maths_headers.h"

namespace phenyl::core {

struct DebugRenderConfig : public IResource {
    bool doPhysicsRender = false;
    bool doProfileRender = true;

    [[nodiscard]] std::string_view getName () const noexcept override {
        return "DebugRuntimeConfig";
    }
};

struct DebugRect {
    glm::vec2 vertices[4];

    static DebugRect AxisAligned (glm::vec2 topLeft, glm::vec2 size) {
        return Create(topLeft, {size.x, 0}, {0, size.y});
    }

    static DebugRect Create (glm::vec2 start, glm::vec2 widthVec, glm::vec2 heightVec) {
        return {
          .vertices = {start, start + widthVec, start + widthVec + heightVec, start + heightVec},
        };
    }
};

class IDebugRenderer {
public:
    virtual ~IDebugRenderer () = default;
    virtual void renderRect (const DebugRect& rect, glm::vec4 color, bool outline, bool worldSpace) = 0;
    virtual void renderText (glm::vec2 pos, std::uint32_t size, glm::vec4 color, std::string text) = 0;
};

class Debug : public IResource {
public:
    void setRenderer (IDebugRenderer* renderer);

    void displayWorldRect (const DebugRect& rect, glm::vec4 color, bool outline = false);
    void displayScreenRect (const DebugRect& rect, glm::vec4 color, bool outline = false);

    void debugText (glm::vec2 pos, std::uint32_t size, std::string text);
    void debugText (glm::vec2 pos, std::uint32_t size, glm::vec4 color, std::string text);

    std::string_view getName () const noexcept override {
        return "Debug";
    }

private:
    IDebugRenderer* m_renderer = nullptr;
};
} // namespace phenyl::core
