#include "debug_layer.h"
#include "graphics/debug_graphics.h"
#include "graphics/pipeline/pipeline.h"
#include "common/assets/assets.h"

#define STARTING_BUFFER_SIZE 256

using namespace phenyl::graphics;

struct DebugBox {
    glm::vec3 vertices[4];
    glm::vec4 colour;
    glm::vec4 outlineColour;

    static DebugBox WorldBox (glm::vec2 pos1, glm::vec2 pos2, glm::vec2 pos3, glm::vec2 pos4, glm::vec4 colour, glm::vec4 outlineColour) {
        return DebugBox {
            .vertices = {glm::vec3{pos1, 1}, glm::vec3{pos2, 1}, glm::vec3{pos3, 1}, glm::vec3{pos4, 1}},
            .colour = colour,
            .outlineColour = outlineColour
        };
    }

    static DebugBox ScreenBox (glm::vec2 pos1, glm::vec2 pos2, glm::vec2 pos3, glm::vec2 pos4, glm::vec4 colour, glm::vec4 outlineColour) {
        return DebugBox {
                .vertices = {glm::vec3{pos1, 0}, glm::vec3{pos2, 0}, glm::vec3{pos3, 0}, glm::vec3{pos4, 0}},
                .colour = colour,
                .outlineColour = outlineColour
        };
    }
};

struct DebugLine {
    glm::vec3 vertices[2];
    glm::vec4 colour;

    static DebugLine WorldLine (glm::vec2 vertex1, glm::vec2 vertex2, glm::vec4 colour) {
        return DebugLine {
            .vertices = {glm::vec3{vertex1, 1}, glm::vec3{vertex2, 1}},
            .colour = colour
        };
    }

    static DebugLine ScreenLine (glm::vec2 vertex1, glm::vec2 vertex2, glm::vec4 colour) {
        return DebugLine {
            .vertices = {glm::vec3{vertex1, 0}, glm::vec3{vertex2, 0}},
            .colour = colour
        };
    }
};

static std::vector<DebugBox> boxes;
static std::vector<DebugLine> lines;

namespace phenyl::graphics {
    class DebugPipeline : public Pipeline<> {
    private:
        PipelineStage boxStage;
        Buffer<glm::vec3> boxPos;
        Buffer<glm::vec4> boxColour;

        PipelineStage lineStage;
        Buffer<glm::vec3> linePos;
        Buffer<glm::vec4> lineColour;

        void bufferBox (const DebugBox& box) {
            for (int i = 0; i < 6; i++) {
                int boxPoint = i != 4 ? (i % 3) : 3;

                boxPos.pushData(box.vertices[boxPoint]);
                boxColour.pushData(box.colour);
            }

            for (int i = 0; i < 4; i++) {
                int nextI = (i + 1) % 4;

                linePos.pushData(box.vertices[i]);
                linePos.pushData(box.vertices[nextI]);
                lineColour.pushData(box.outlineColour);
                lineColour.pushData(box.outlineColour);
            }
        }

        void bufferLine (const DebugLine& line) {
            linePos.pushData(line.vertices[0]);
            linePos.pushData(line.vertices[1]);
            lineColour.pushData(line.colour);
            lineColour.pushData(line.colour);
        }
    public:
        explicit DebugPipeline () {}

        void init (graphics::Renderer* renderer) override {
            auto shader = common::Assets::Load<Shader>("resources/shaders/debug");
            boxPos = renderer->makeBuffer<glm::vec3>(STARTING_BUFFER_SIZE);
            boxColour = renderer->makeBuffer<glm::vec4>(STARTING_BUFFER_SIZE);
            linePos = renderer->makeBuffer<glm::vec3>(STARTING_BUFFER_SIZE);
            lineColour = renderer->makeBuffer<glm::vec4>(STARTING_BUFFER_SIZE);

            boxStage = renderer->buildPipelineStage(PipelineStageBuilder{shader}
                                                            .addVertexAttrib<glm::vec3>(0)
                                                            .addVertexAttrib<glm::vec4>(1));
            boxStage.bindBuffer(0, boxPos);
            boxStage.bindBuffer(1, boxColour);

            lineStage = renderer->buildPipelineStage(PipelineStageBuilder{std::move(shader)}
                                                            .withPipelineType(PipelineType::LINES)
                                                            .addVertexAttrib<glm::vec3>(0)
                                                            .addVertexAttrib<glm::vec4>(1));
            lineStage.bindBuffer(0, linePos);
            lineStage.bindBuffer(1, lineColour);
        }

        void bufferData () override {
            boxStage.clearBuffers();
            lineStage.clearBuffers();
            for (const auto& i : boxes) {
                bufferBox(i);
            }

            for (const auto& i : lines) {
                bufferLine(i);
            }

            boxStage.bufferAllData();
            lineStage.bufferAllData();
            boxes.clear();
            lines.clear();
        }

        void render () override {
            boxStage.render();
            lineStage.render();
        }

        void applyCamera (const Camera& camera) {
            boxStage.applyUniform(Camera::getUniformName(), camera.getCamMatrix());
            lineStage.applyUniform(Camera::getUniformName(), camera.getCamMatrix());
        }

        void applyScreenSize (glm::vec2 screenSize) {
            auto screenTransform = glm::scale(glm::vec3{2 / screenSize.x, 2 / screenSize.y, 1}) * glm::translate(glm::vec3{-1, -1, 0});
            boxStage.applyUniform("screenTransform", screenTransform);
            lineStage.applyUniform("screenTransform", screenTransform);
        }
    };
}

DebugLayer::DebugLayer (Renderer* renderer) : pipeline{}, screenSize{renderer->getScreenSize()} {
    pipeline = std::make_unique<graphics::DebugPipeline>();
    pipeline->init(renderer);
}

std::string DebugLayer::getName () {
    return "debug_layer";
}

int DebugLayer::getPriority () {
    return 4;
}

bool DebugLayer::isActive () {
    return active;
}

void DebugLayer::gatherData () {

}

void DebugLayer::preRender (graphics::Renderer* renderer) {
    pipeline->bufferData();
}

int DebugLayer::getUniformId (std::string uniformName) {
    logging::log(LEVEL_WARNING, "The debug pipeline does not accept uniform \"{}\"!", uniformName);
    return 0;
}

void DebugLayer::applyUniform (int uniformId, void* data) {
    logging::log(LEVEL_WARNING, "The debug pipeline does not accept uniforms!");
}

void DebugLayer::applyCamera (graphics::Camera camera) {
    pipeline->applyCamera(camera);
    pipeline->applyScreenSize(screenSize);
}

void DebugLayer::render (graphics::Renderer* renderer, graphics::FrameBuffer* frameBuf) {
    pipeline->render();
    boxes.clear();
    lines.clear();
}

std::shared_ptr<DebugLayer> phenyl::graphics::makeDebugLayer (graphics::Renderer* renderer) {
    return std::make_shared<DebugLayer>(renderer);
}

/*void graphics::debugWorldRect (glm::vec2 topLeft, glm::vec2 bottomRight, glm::vec4 colour, glm::vec4 outlineColour) {
    boxes.push_back(DebugBox::WorldBox(topLeft, bottomRight, colour, outlineColour));
}


void graphics::debugScreenRect (glm::vec2 topLeft, glm::vec2 bottomRight, glm::vec4 colour, glm::vec4 outlineColour) {
    boxes.push_back(DebugBox::ScreenBox(topLeft, bottomRight, colour, outlineColour));
}*/
void phenyl::graphics::debugWorldRect (glm::vec2 pos1, glm::vec2 pos2, glm::vec2 pos3, glm::vec2 pos4, glm::vec4 colour, glm::vec4 outlineColour) {
    boxes.push_back(DebugBox::WorldBox(pos1, pos2, pos3, pos4, colour, outlineColour));
}

void phenyl::graphics::debugScreenRect (glm::vec2 pos1, glm::vec2 pos2, glm::vec2 pos3, glm::vec2 pos4, glm::vec4 colour, glm::vec4 outlineColour) {
    boxes.push_back(DebugBox::ScreenBox(pos1, pos2, pos3, pos4, colour, outlineColour));
}

void phenyl::graphics::debugWorldLine (glm::vec2 start, glm::vec2 end, glm::vec4 colour) {
    lines.push_back(DebugLine::WorldLine(start, end, colour));
}

void phenyl::graphics::debugScreenLine (glm::vec2 start, glm::vec2 end, glm::vec4 colour) {
    lines.push_back(DebugLine::ScreenLine(start, end, colour));
}