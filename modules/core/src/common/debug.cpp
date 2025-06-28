#include "core/debug.h"

#include "logging/logging.h"

#include <utility>

using namespace phenyl;

void core::Debug::setRenderer (IDebugRenderer* renderer) {
    m_renderer = renderer;
}

void core::Debug::displayWorldRect (const DebugRect& rect, glm::vec4 color, bool outline) {
    if (!m_renderer) {
        return;
    }

    m_renderer->renderRect(rect, color, outline, true);
}

void core::Debug::displayScreenRect (const DebugRect& rect, glm::vec4 color, bool outline) {
    if (!m_renderer) {
        return;
    }

    m_renderer->renderRect(rect, color, outline, false);
}

void core::Debug::debugText (glm::vec2 pos, std::uint32_t size, std::string text) {
    debugText(pos, size, glm::vec4{1.0, 1.0, 1.0, 0.0}, std::move(text));
}

void core::Debug::debugText (glm::vec2 pos, std::uint32_t size, glm::vec4 color, std::string text) {
    if (!m_renderer) {
        return;
    }

    m_renderer->renderText(pos, size, color, std::move(text));
}
