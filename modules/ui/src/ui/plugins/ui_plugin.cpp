#include "ui/plugins/ui_plugin.h"

#include "core/assets/assets.h"
#include "core/debug.h"
#include "core/input/game_input.h"
#include "core/plugins/input_plugin.h"
#include "core/runtime.h"
#include "graphics/camera_2d.h"
#include "graphics/plugins/graphics_plugin.h"

using namespace phenyl::graphics;

class UIDebugRenderer : public phenyl::core::IDebugRenderer, public phenyl::core::IResource {
public:
    void renderRect (const phenyl::core::DebugRect& rect, glm::vec4 color, bool outline, bool worldSpace) override {
        if (worldSpace) {
            m_worldRectRequests.emplace_back(rect, color, outline);
        } else {
            m_screenRectRequests.emplace_back(rect, color, outline);
        }
    }

    void renderText (glm::vec2 pos, std::uint32_t size, glm::vec4 color, std::string text) override {
        m_textRequests.emplace_back(pos, size, color, std::move(text));
    }

    void renderScreen (Canvas& canvas) {
        for (const auto& req : m_screenRectRequests) {
            CanvasStyle style{
              .fill = req.outline ? CanvasFill::OUTLINE : CanvasFill::FILLED,
              .outlineSize = 2.0f,
              .colour = req.color,
              .useAA = false,
            };
            canvas.renderPolygon(req.rect.vertices, style);
        }
        m_screenRectRequests.clear();

        for (const auto& req : m_textRequests) {
            canvas.renderText(req.pos, canvas.defaultFont(), req.size, req.text, req.color);
        }
        m_textRequests.clear();
    }

    void renderWorld2D (Canvas& canvas, const Camera2D& camera) {
        auto camMat = camera.getCamMatrix();
        glm::mat4 unitTransformMat{{0.5, 0, 0, 0}, {0, -0.5, 0, 0}, {0, 0, 1.0, 0}, {0.5, 0.5, 0, 1.0}};
        glm::mat4 screenMat{{canvas.resolution().x, 0, 0, 0}, {0, canvas.resolution().y, 0, 0}, {0, 0, 1, 0},
          {0, 0, 0, 1}};
        auto mat = screenMat * unitTransformMat * camMat;
        for (const auto& req : m_worldRectRequests) {
            CanvasStyle style{
              .fill = req.outline ? CanvasFill::OUTLINE : CanvasFill::FILLED,
              .outlineSize = 2.0f,
              .colour = req.color,
              .useAA = false,
            };
            glm::vec2 vertices[4] = {};
            for (std::size_t i = 0; i < 4; i++) {
                auto v = req.rect.vertices[i];
                auto vec = mat * glm::vec4{v, 0, 1};
                vertices[i] = {vec.x, vec.y};
            }
            canvas.renderPolygon(vertices, style);
        }
        m_worldRectRequests.clear();
    }

    std::string_view getName () const noexcept override {
        return "UIDebugRenderer";
    }

private:
    struct RectRequest {
        phenyl::core::DebugRect rect;
        glm::vec4 color;
        bool outline;
    };

    struct TextRequest {
        glm::vec2 pos;
        std::uint32_t size;
        glm::vec4 color;
        std::string text;
    };

    std::vector<RectRequest> m_screenRectRequests;
    std::vector<RectRequest> m_worldRectRequests;
    std::vector<TextRequest> m_textRequests;
};

static void UIUpdateSystem (const phenyl::core::Resources<UIManager>& resources) {
    resources.get<UIManager>().updateUI();
}

static void DebugScreenRenderSystem (const phenyl::core::Resources<UIDebugRenderer, Canvas>& resources) {
    auto& [debugRenderer, canvas] = resources;
    debugRenderer.renderScreen(canvas);
}

static void DebugWorld2DRenderSystem (
    const phenyl::core::Resources<UIDebugRenderer, Canvas, const Camera2D>& resources) {
    auto& [debugRenderer, canvas, camera] = resources;
    debugRenderer.renderWorld2D(canvas, camera);
}

static void UIRenderSystem (const phenyl::core::Resources<UIManager, Canvas>& resources) {
    auto& [manager, canvas] = resources;
    manager.renderUI(canvas);
}

std::string_view UIPlugin::getName () const noexcept {
    return "UIPlugin";
}

void UIPlugin::init (phenyl::core::PhenylRuntime& runtime) {
    runtime.addPlugin<GraphicsPlugin>();
    runtime.addPlugin<core::InputPlugin>();

    auto& renderer = runtime.resource<Renderer>();
    auto& input = runtime.resource<core::GameInput>();
    m_manager = std::make_unique<UIManager>(input);

    runtime.addResource<Canvas>(renderer);
    runtime.addResource(m_manager.get());

    runtime.addResource<UIDebugRenderer>();
    auto& debugRenderer = runtime.resource<UIDebugRenderer>();
    runtime.resource<core::Debug>().setRenderer(&debugRenderer);

    runtime.addSystem<core::FrameBegin>("UIManager::Update", UIUpdateSystem);

    auto& debugScreenRender =
        runtime.addSystem<core::Render>("UIDebugRenderer::DebugScreenRender", DebugScreenRenderSystem);
    auto& debugWorld2DRender =
        runtime.addSystem<core::Render>("UIDebugRenderer::DebugWorld2DRender", DebugWorld2DRenderSystem);
    debugScreenRender.runAfter(debugWorld2DRender);

    auto& uiRender = runtime.addSystem<core::Render>("UIManager::Render", UIRenderSystem);
    uiRender.runAfter(debugScreenRender);
    uiRender.runAfter(debugWorld2DRender);
}
