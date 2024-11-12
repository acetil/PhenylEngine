#include "graphics/font/font_manager.h"
#include "graphics/renderlayer/canvas_layer.h"
#include "graphics/canvas/canvas.h"

#include "common/assets/assets.h"

using namespace phenyl::graphics;

static phenyl::Logger LOGGER{"CANVAS", detail::GRAPHICS_LOGGER};

static void addCurve (std::vector<glm::vec2>& points, glm::vec2 start, glm::vec2 end, glm::vec2 centre, unsigned int quanta) {
    auto d1 = start - centre;
    auto d2 = end - centre;

    for (int i = 0; i < quanta + 2; i++) {
        auto theta = static_cast<float>(i) / static_cast<float>(quanta + 1) * glm::pi<float>() / 2;
        points.emplace_back(centre + d1 * glm::cos(theta) + d2 * glm::sin(theta));
    }
}

void Canvas::submitVertices (std::span<glm::vec2> vertices, const CanvasStyle& style) {
    if (style.fill == CanvasFill::FILLED) {
        if (style.useAA) {
            layer.renderConvexPolyAA(vertices, style.colour);
        } else {
            layer.renderConvexPoly(vertices, style.colour);
        }
    } else {
        if (style.useAA) {
            layer.renderPolyLineAA(vertices, style.colour, style.outlineSize, true);
        } else {
            layer.renderPolyLine(vertices, style.colour, style.outlineSize, true);
        }
    }
}

glm::vec2 Canvas::offset () const {
    PHENYL_DASSERT(!offsetStack.empty());
    return offsetStack.back();
}

void Canvas::onViewportResize (glm::ivec2 oldResolution, glm::ivec2 newResolution) {
    canvasResolution = newResolution;
    layer.setScreenSize(newResolution);
}

Canvas::Canvas (Renderer& renderer) : atlas{renderer}, layer{renderer.addLayer<CanvasRenderLayer>(atlas)}, fontManager{std::make_unique<FontManager>(renderer.getViewport(), atlas)} {
    fontManager->selfRegister();
    canvasResolution = renderer.getViewport().getResolution();
    layer.setScreenSize(canvasResolution);
    renderer.getViewport().addUpdateHandler(this);
    offsetStack.emplace_back(0, 0);

    defaultFontAsset = common::Assets::Load<Font>("resources/phenyl/fonts/noto-serif");
}
Canvas::~Canvas () = default;

void Canvas::render (glm::vec2 pos, const CanvasRect& rect, const CanvasStyle& style) {
    auto offPos = offset() + pos;
    glm::vec2 vertices[] = {
        offPos, offPos + glm::vec2{rect.size.x, 0}, offPos + glm::vec2{rect.size}, offPos + glm::vec2{0, rect.size.y}
    };
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
    addCurve(vertices, offPos + glm::vec2{0, rect.rounding}, offPos + glm::vec2{rect.rounding, 0}, offPos + glm::vec2{rect.rounding, rect.rounding}, style.quanta / 4);
    addCurve(vertices, offPos + glm::vec2{size.x - rect.rounding, 0.0f}, offPos + glm::vec2{size.x, rect.rounding}, offPos + glm::vec2{size.x - rect.rounding, rect.rounding}, style.quanta / 4);
    addCurve(vertices, offPos + glm::vec2{size.x, size.y - rect.rounding}, offPos + glm::vec2{size.x - rect.rounding, size.y}, offPos + glm::vec2{size.x - rect.rounding, size.y - rect.rounding}, style.quanta / 4);
    addCurve(vertices, offPos + glm::vec2{rect.rounding, size.y}, offPos + glm::vec2{0.0f, size.y - rect.rounding}, offPos + glm::vec2{rect.rounding, size.y - rect.rounding}, style.quanta / 4);

    submitVertices(vertices, style);
}

void Canvas::renderText (glm::vec2 pos, common::Asset<Font>& font, std::uint32_t size, std::string_view text, glm::vec3 colour) {
    font->renderText(layer, size, text, offset() + pos, colour);
}

void Canvas::pushOffset (glm::vec2 off) {
    PHENYL_DASSERT(!offsetStack.empty());
    offsetStack.emplace_back(offsetStack.back() + off);
}

void Canvas::popOffset () {
    if (offsetStack.size() == 1) {
        PHENYL_LOGE(LOGGER, "Attempted to pop offset off empty offset stack!");
        return;
    }

    offsetStack.pop_back();
}

std::string_view Canvas::getName () const noexcept {
    return "Canvas";
}


