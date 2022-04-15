#include "graphics/renderlayer/ui_layer.h"

#include "graphics/font/font.h"
#include "graphics/font/rendered_text.h"

#include "logging/logging.h"

using namespace graphics;

#define BUFFER_SIZE (2000 * 6)

class UIPipelineInt : public UIPipeline {
private:
    PipelineStage textStage;
    ShaderProgramNew shader;
    GraphicsTexture fontTexture;

    Buffer<glm::vec2> textPosBuffer;
    Buffer<glm::vec2> textUvBuffer;
    Buffer<glm::vec3> textColourBuffer;
public:
    UIPipelineInt (const ShaderProgramNew& _shader, GraphicsTexture _fontTexture) : shader{_shader}, fontTexture{_fontTexture} {}
    void init (Renderer* renderer) override {
        textStage = renderer->buildPipelineStage(PipelineStageBuilder(shader)
                .addVertexAttrib<glm::vec2>(0)
                .addVertexAttrib<glm::vec2>(1)
                .addVertexAttrib<glm::vec3>(2));

        textPosBuffer = renderer->makeBuffer<glm::vec2>(BUFFER_SIZE);
        textUvBuffer = renderer->makeBuffer<glm::vec2>(BUFFER_SIZE);
        textColourBuffer = renderer->makeBuffer<glm::vec3>(BUFFER_SIZE);

        textStage.bindBuffer(0, textPosBuffer);
        textStage.bindBuffer(1, textUvBuffer);
        textStage.bindBuffer(2, textColourBuffer);
    }

    void bufferData () override {
        textStage.bufferAllData();
    }

    void render () override {
        fontTexture.bindTexture();
        textStage.render();
        textStage.clearBuffers();
    }

    void bufferText(const RenderedText &text) override {
        textPosBuffer.pushData(text.getPosComp().cbegin(), text.getPosComp().cend());
        textUvBuffer.pushData(text.getUvComp().cbegin(), text.getUvComp().cend());
        textColourBuffer.pushData(text.getColourComp().cbegin(), text.getColourComp().cend());
    }
};

std::string graphics::UIRenderLayer::getName () {
    return "ui_layer";
}

int graphics::UIRenderLayer::getPriority () {
    return 2;
}

bool graphics::UIRenderLayer::isActive () {
    return true;
}

void graphics::UIRenderLayer::gatherData () {

}

void graphics::UIRenderLayer::preRender (graphics::Renderer* renderer) {
    pipeline->bufferData();
}

int graphics::UIRenderLayer::getUniformId (std::string uniformName) {
    return 0;
}

void graphics::UIRenderLayer::applyUniform (int uniformId, void* data) {

}

void graphics::UIRenderLayer::applyCamera (graphics::Camera camera) {

}

void graphics::UIRenderLayer::render (graphics::Renderer* renderer, graphics::FrameBuffer* frameBuf) {
    /*textProgram.bind();

    fontTexture.bindTexture();

    int size = textBuffer->currentSize() / 3 / 4 / 2;

    renderer->bufferData(textIds, textBuffer);

    renderer->render(textIds, textProgram, size);*/
    pipeline->render();
}

void graphics::UIRenderLayer::bufferStr (graphics::Font& font, const std::string& text, int size, int x, int y, glm::vec3 colour) {
    //font.renderText(text, size, x, y, colour, textBuffer);
}

UIRenderLayer::UIRenderLayer (GraphicsTexture _fontTexture, Renderer* renderer) : fontTexture(_fontTexture),
                                                                                  textProgram(renderer->getProgramNew(
                                                                                          "text").orThrow()) {
    pipeline = std::make_unique<UIPipelineInt>(textProgram, fontTexture);
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







