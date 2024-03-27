#include "debug_layer.h"
#include "graphics/debug_graphics.h"
#include "common/assets/assets.h"

#define STARTING_BUFFER_SIZE 2048

using namespace phenyl::graphics;

static phenyl::Logger LOGGER{"DEBUG_LAYER", detail::GRAPHICS_LOGGER};

namespace phenyl::graphics {
    struct DebugBox {
        glm::vec3 vertices[4];
        glm::vec4 colour;
        glm::vec4 outlineColour;

        static DebugBox WorldBox (glm::vec2 pos1, glm::vec2 pos2, glm::vec2 pos3, glm::vec2 pos4, glm::vec4 colour,
                                  glm::vec4 outlineColour) {
            return DebugBox{
                    .vertices = {glm::vec3{pos1, 1}, glm::vec3{pos2, 1}, glm::vec3{pos3, 1}, glm::vec3{pos4, 1}},
                    .colour = colour,
                    .outlineColour = outlineColour
            };
        }

        static DebugBox ScreenBox (glm::vec2 pos1, glm::vec2 pos2, glm::vec2 pos3, glm::vec2 pos4, glm::vec4 colour,
                                   glm::vec4 outlineColour) {
            return DebugBox{
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
            return DebugLine{
                    .vertices = {glm::vec3{vertex1, 1}, glm::vec3{vertex2, 1}},
                    .colour = colour
            };
        }

        static DebugLine ScreenLine (glm::vec2 vertex1, glm::vec2 vertex2, glm::vec4 colour) {
            return DebugLine{
                    .vertices = {glm::vec3{vertex1, 0}, glm::vec3{vertex2, 0}},
                    .colour = colour
            };
        }
    };
}

static std::vector<DebugBox> boxes;
static std::vector<DebugLine> lines;

DebugLayer::DebugLayer () : AbstractRenderLayer{1} {}

std::string_view DebugLayer::getName () const {
    return "DebugLayer";
}

void DebugLayer::init (Renderer& renderer) {
    auto shader = common::Assets::Load<Shader>("phenyl/shaders/debug");
    boxPos = renderer.makeBuffer<glm::vec3>(STARTING_BUFFER_SIZE);
    boxColour = renderer.makeBuffer<glm::vec4>(STARTING_BUFFER_SIZE);
    linePos = renderer.makeBuffer<glm::vec3>(STARTING_BUFFER_SIZE);
    lineColour = renderer.makeBuffer<glm::vec4>(STARTING_BUFFER_SIZE);
    uniformBuffer = renderer.makeUniformBuffer<Uniform>();

    BufferBinding posBinding;
    BufferBinding colourBinding;
    UniformBinding uniformBinding;
    boxPipeline = renderer.buildPipeline()
                          .withShader(shader)
                          .withBuffer<glm::vec3>(posBinding)
                          .withBuffer<glm::vec4>(colourBinding)
                          .withAttrib<glm::vec3>(0, posBinding)
                          .withAttrib<glm::vec4>(1, colourBinding)
                          .withUniform<Uniform>(shader->getUniformLocation("Uniform"), uniformBinding)
                          .build();
    boxPipeline.bindBuffer(posBinding, boxPos);
    boxPipeline.bindBuffer(colourBinding, boxColour);
    boxPipeline.bindUniform(uniformBinding, uniformBuffer);

    linePipeline = renderer.buildPipeline()
                           .withGeometryType(GeometryType::LINES)
                           .withShader(shader)
                           .withBuffer<glm::vec3>(posBinding)
                           .withBuffer<glm::vec4>(colourBinding)
                           .withAttrib<glm::vec3>(0, posBinding)
                           .withAttrib<glm::vec4>(1, colourBinding)
                           .withUniform<Uniform>(shader->getUniformLocation("Uniform"), uniformBinding)
                           .build();
    linePipeline.bindBuffer(posBinding, linePos);
    linePipeline.bindBuffer(colourBinding, lineColour);
    linePipeline.bindUniform(uniformBinding, uniformBuffer);
}

void DebugLayer::bufferData (const Camera& camera, glm::vec2 screenSize) {
    boxPos.clear();
    boxColour.clear();
    linePos.clear();
    lineColour.clear();

    for (const auto& i : boxes) {
        bufferBox(i);
    }

    for (const auto& i : lines) {
        bufferLine(i);
    }

    boxPos.upload();
    boxColour.upload();
    linePos.upload();
    lineColour.upload();

    boxes.clear();
    lines.clear();

    uniformBuffer->camera = camera.getCamMatrix();
    uniformBuffer->screenTransform = glm::scale(glm::vec3{2 / screenSize.x, 2 / screenSize.y, 1}) * glm::translate(glm::vec3{-1, -1, 0});
}

void DebugLayer::render () {
    boxPipeline.render(boxPos.size());
    linePipeline.render(linePos.size());
}

void DebugLayer::bufferBox (const DebugBox& box) {
    for (int i = 0; i < 6; i++) {
        int boxPoint = i != 4 ? (i % 3) : 3;

        boxPos.emplace(box.vertices[boxPoint]);
        boxColour.emplace(box.colour);
    }

    for (int i = 0; i < 4; i++) {
        int nextI = (i + 1) % 4;

        linePos.emplace(box.vertices[i]);
        linePos.emplace(box.vertices[nextI]);
        lineColour.emplace(box.outlineColour);
        lineColour.emplace(box.outlineColour);
    }
}

void DebugLayer::bufferLine (const DebugLine& line) {
    linePos.emplace(line.vertices[0]);
    linePos.emplace(line.vertices[1]);
    lineColour.emplace(line.colour);
    lineColour.emplace(line.colour);
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