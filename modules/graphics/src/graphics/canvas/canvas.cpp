#include "graphics/canvas/canvas.h"

#include "core/assets/assets.h"
#include "graphics/detail/loggers.h"
#include "graphics/font/font_manager.h"
#include "graphics/renderlayer/canvas_layer.h"

using namespace phenyl::graphics;

static phenyl::Logger LOGGER{"CANVAS", detail::GRAPHICS_LOGGER};

static void addCurve (std::vector<glm::vec2>& points, glm::vec2 start, glm::vec2 end, glm::vec2 centre,
    unsigned int quanta) {
    auto d1 = start - centre;
    auto d2 = end - centre;

    for (int i = 0; i < quanta + 2; i++) {
        auto theta = static_cast<float>(i) / static_cast<float>(quanta + 1) * glm::pi<float>() / 2;
        points.emplace_back(centre + d1 * glm::cos(theta) + d2 * glm::sin(theta));
    }
}

void Canvas::submitVertices (std::span<const glm::vec2> vertices, const CanvasStyle& style) {
    if (style.fill == CanvasFill::FILLED) {
        if (style.useAA) {
            m_layer.renderConvexPolyAA(vertices, style.colour);
        } else {
            m_layer.renderConvexPoly(vertices, style.colour);
        }
    } else {
        if (style.useAA) {
            m_layer.renderPolyLineAA(vertices, style.colour, style.outlineSize, true);
        } else {
            m_layer.renderPolyLine(vertices, style.colour, style.outlineSize, true);
        }
    }
}

glm::vec2 Canvas::offset () const {
    PHENYL_DASSERT(!m_offsetStack.empty());
    return m_offsetStack.back();
}

void Canvas::onViewportResize (glm::ivec2 oldResolution, glm::ivec2 newResolution) {
    m_resolution = newResolution;
    m_layer.setScreenSize(newResolution);
}

Canvas::Canvas (Renderer& renderer) :
    m_atlas{renderer},
    m_layer{renderer.addLayer<CanvasRenderLayer>(m_atlas)},
    m_fontManager{std::make_unique<FontManager>(renderer.getViewport(), m_atlas)} {
    m_fontManager->selfRegister();
    m_resolution = renderer.getViewport().getResolution();
    m_layer.setScreenSize(m_resolution);
    renderer.getViewport().addUpdateHandler(this);
    m_offsetStack.emplace_back(0, 0);

    m_defaultFont = core::Assets::Load2<Font>("resources/phenyl/fonts/noto-serif");
}

Canvas::~Canvas () = default;

void Canvas::render (glm::vec2 pos, const CanvasRect& rect, const CanvasStyle& style) {
    auto offPos = offset() + pos;
    glm::vec2 vertices[] = {offPos, offPos + glm::vec2{rect.size.x, 0}, offPos + glm::vec2{rect.size},
      offPos + glm::vec2{0, rect.size.y}};
    submitVertices(vertices, style);
}

void Canvas::render (glm::vec2 pos, const CanvasRoundedRect& rect, const CanvasStyle& style) {
    if (rect.rounding <= 0.0f) {
        render(pos, rect.rect, style);
        return;
    }
    auto offPos = offset() + pos;

    std::vector<glm::vec2> vertices;

    auto size = rect.rect.size;
    addCurve(vertices, offPos + glm::vec2{0, rect.rounding}, offPos + glm::vec2{rect.rounding, 0},
        offPos + glm::vec2{rect.rounding, rect.rounding}, style.quanta / 4);
    addCurve(vertices, offPos + glm::vec2{size.x - rect.rounding, 0.0f}, offPos + glm::vec2{size.x, rect.rounding},
        offPos + glm::vec2{size.x - rect.rounding, rect.rounding}, style.quanta / 4);
    addCurve(vertices, offPos + glm::vec2{size.x, size.y - rect.rounding},
        offPos + glm::vec2{size.x - rect.rounding, size.y},
        offPos + glm::vec2{size.x - rect.rounding, size.y - rect.rounding}, style.quanta / 4);
    addCurve(vertices, offPos + glm::vec2{rect.rounding, size.y}, offPos + glm::vec2{0.0f, size.y - rect.rounding},
        offPos + glm::vec2{rect.rounding, size.y - rect.rounding}, style.quanta / 4);

    submitVertices(vertices, style);
}

void Canvas::renderText (glm::vec2 pos, const std::shared_ptr<Font>& font, std::uint32_t size, std::string_view text,
    glm::vec3 colour) {
    font->renderText(m_layer, size, text, offset() + pos, colour);
}

void Canvas::renderPolygon (std::span<const glm::vec2> vertices, const CanvasStyle& style) {
    submitVertices(vertices, style);
}

void Canvas::pushOffset (glm::vec2 off) {
    PHENYL_DASSERT(!m_offsetStack.empty());
    m_offsetStack.emplace_back(m_offsetStack.back() + off);
}

void Canvas::popOffset () {
    if (m_offsetStack.size() == 1) {
        PHENYL_LOGE(LOGGER, "Attempted to pop offset off empty offset stack!");
        return;
    }

    m_offsetStack.pop_back();
}

std::string_view Canvas::getName () const noexcept {
    return "Canvas";
}
