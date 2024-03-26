#include "graphics/renderlayer/ui_layer.h"

#include "graphics/font/font.h"
#include "graphics/font/rendered_text.h"
#include "common/assets/assets.h"

#include "logging/logging.h"

using namespace phenyl::graphics;

#define BUFFER_SIZE (256 * 6)

namespace {
    struct Uniform {
        glm::vec2 screenSize;
    };

    struct TextVertex {
        glm::vec2 pos;
        glm::vec2 uv;
        glm::vec3 colour;
    };

    struct BoxVertex {
        glm::vec2 pos;
        glm::vec2 rectPos;
        glm::vec4 borderColour;
        glm::vec4 bgColour;
        glm::vec4 details;
    };
}

class UIPipelineInt : public UIPipeline {
private:
    Pipeline2 textPipeline;
    GraphicsTexture fontTexture;
    SamplerBinding samplerBinding;
    Buffer<TextVertex> textBuffer;

    Pipeline2 boxPipeline;
    Buffer<BoxVertex> boxBuffer;

    UniformBinding uniformBinding;
    UniformBuffer<Uniform> uniformBuffer;

    glm::vec2 screenSize = {800, 600};
public:
    UIPipelineInt (GraphicsTexture _fontTexture) : fontTexture{std::move(_fontTexture)} {}
    void init (Renderer* renderer) override {
        BufferBinding textBinding;
        textPipeline = renderer->buildPipeline()
                .withShader(phenyl::common::Assets::Load<Shader>("phenyl/shaders/text"))
                .withBuffer<TextVertex>(textBinding)
                .withAttrib<glm::vec2>(0, textBinding, offsetof(TextVertex, pos))
                .withAttrib<glm::vec2>(1, textBinding, offsetof(TextVertex, uv))
                .withAttrib<glm::vec3>(2, textBinding, offsetof(TextVertex, colour))
                .withSampler2D(0, samplerBinding)
                .build();


        textBuffer = renderer->makeBuffer<TextVertex>(BUFFER_SIZE);

        textPipeline.bindBuffer(textBinding, textBuffer);
        textPipeline.bindSampler(samplerBinding, fontTexture);

        BufferBinding boxBinding;

        auto shader = phenyl::common::Assets::Load<Shader>("phenyl/shaders/box");
        boxPipeline = renderer->buildPipeline()
                .withShader(shader)
                .withBuffer<BoxVertex>(boxBinding)
                .withAttrib<glm::vec2>(0, boxBinding, offsetof(BoxVertex, pos))
                .withAttrib<glm::vec2>(1, boxBinding, offsetof(BoxVertex, rectPos))
                .withAttrib<glm::vec4>(2, boxBinding, offsetof(BoxVertex, borderColour))
                .withAttrib<glm::vec4>(3, boxBinding, offsetof(BoxVertex, bgColour))
                .withAttrib<glm::vec4>(4, boxBinding, offsetof(BoxVertex, details))
                .withUniform<Uniform>(shader->getUniformLocation("Uniform"), uniformBinding)
                .build();

        boxBuffer = renderer->makeBuffer<BoxVertex>(BUFFER_SIZE);
        uniformBuffer = renderer->makeUniformBuffer<Uniform>(glm::vec2{800, 600});

        boxPipeline.bindBuffer(boxBinding, boxBuffer);
        boxPipeline.bindUniform(uniformBinding, uniformBuffer);
    }

    void bufferData () override {
        textBuffer.upload();
        boxBuffer.upload();
    }

    void render () override {
        textPipeline.bindSampler(samplerBinding, fontTexture);
        textPipeline.render(textBuffer.size());

        boxPipeline.bindUniform(uniformBinding, uniformBuffer);
        boxPipeline.render(boxBuffer.size());

        clearBuffers();
    }

    void bufferText(const RenderedText& text) override {
        auto posIt = text.getPosComp().cbegin();
        auto uvIt = text.getUvComp().cbegin();
        auto colourIt = text.getColourComp().cbegin();

        while (posIt != text.getPosComp().cend()) {
            textBuffer.emplace(TextVertex{
                .pos = *posIt,
                .uv = *uvIt,
                .colour = *colourIt
            });

            ++posIt;
            ++uvIt;
            ++colourIt;
        }
    }

    void bufferRect(const UIRect &rect) override {
        for (int i = 0; i < 6; i++) {
            int vertexNum = i == 3 ? i : i % 3;

            glm::vec2 pos{rect.screenPos.x + (float)(vertexNum % 2 == 1) * rect.size.x, rect.screenPos.y + (float)(vertexNum / 2 == 1) * rect.size.y};
            glm::vec2 rectPos{-1.0f + (float)(vertexNum % 2 == 1) * 2.0f, -1.0f + (float)(vertexNum / 2 == 1) * 2.0f};

            boxBuffer.emplace(BoxVertex{
                .pos = pos,
                .rectPos = rectPos,
                .borderColour = rect.borderColour,
                .bgColour = rect.bgColour,
                .details = rect.details
            });
        }
    }

    void setScreenSize(glm::vec2 _screenSize) override {
        uniformBuffer->screenSize = _screenSize;
    }

    void clearBuffers () {
        textBuffer.clear();
        boxBuffer.clear();
    }
};

std::string UIRenderLayer::getName () {
    return "ui_layer";
}

int UIRenderLayer::getPriority () {
    return 3;
}

bool UIRenderLayer::isActive () {
    return true;
}

void UIRenderLayer::gatherData () {

}

void UIRenderLayer::preRender (graphics::Renderer* renderer) {
    pipeline->bufferData();
}

int UIRenderLayer::getUniformId (std::string uniformName) {
    return 0;
}

void UIRenderLayer::applyUniform (int uniformId, void* data) {

}

void UIRenderLayer::applyCamera (graphics::Camera camera) {

}

void UIRenderLayer::render (graphics::Renderer* renderer, graphics::FrameBuffer* frameBuf) {
    /*textProgram.bind();

    fontTexture.bindTexture();

    int size = textBuffer->currentSize() / 3 / 4 / 2;

    renderer->bufferData(textIds, textBuffer);

    renderer->render(textIds, textProgram, size);*/
    pipeline->render();
}

void UIRenderLayer::bufferStr (graphics::Font& font, const std::string& text, int size, int x, int y, glm::vec3 colour) {
    //font.renderText(text, size, x, y, colour, textBuffer);
}

UIRenderLayer::UIRenderLayer (GraphicsTexture fontTexture, Renderer* renderer) {
    pipeline = std::make_unique<UIPipelineInt>(std::move(fontTexture));
    pipeline->init(renderer);
    /*textIds = renderer->getBufferIds(3, 200 * 12 * sizeof(float), {2, 2, 3});
    textBuffer[0] = Buffer(200 * 12, sizeof(float), false);
    textBuffer[1] = Buffer(200 * 12, sizeof(float), false);
    textBuffer[2] = Buffer(200 * 18, sizeof(float), false);*/
}

void UIRenderLayer::bufferText (const RenderedText& text) {
    /*textBuffer[0].pushData((float*)text.getPosComp().begin(), text.getPosComp().size() * 2);
    textBuffer[1].pushData((float*)text.getUvComp().begin(), text.getPosComp().size() * 2);
    textBuffer[2].pushData((float*)text.getColourComp().begin(), text.getPosComp().size() * 3);*/
    pipeline->bufferText(text);
}

void UIRenderLayer::bufferRect (const UIRect& rect) {
    pipeline->bufferRect(rect);
}

void UIRenderLayer::setScreenSize (glm::vec2 screenSize) {
    pipeline->setScreenSize(screenSize);
}







