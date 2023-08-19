#include "graphics/renderlayer/ui_layer.h"

#include "graphics/font/font.h"
#include "graphics/font/rendered_text.h"

#include "logging/logging.h"

using namespace graphics;

#define BUFFER_SIZE (2000 * 6)

class UIPipelineInt : public UIPipeline {
private:
    PipelineStage textStage;
    ShaderProgramNew textShader;
    GraphicsTexture fontTexture;

    Buffer<glm::vec2> textPosBuffer;
    Buffer<glm::vec2> textUvBuffer;
    Buffer<glm::vec3> textColourBuffer;

    PipelineStage boxStage;
    ShaderProgramNew boxShader;

    Buffer<glm::vec2> boxPosBuffer;
    Buffer<glm::vec2> boxRectPosBuffer;
    Buffer<glm::vec4> boxBorderBuffer;
    Buffer<glm::vec4> boxBgBuffer;
    Buffer<glm::vec4> boxDetailsBuffer;

    glm::vec2 screenSize = {800, 600};
public:
    UIPipelineInt (const ShaderProgramNew& _textShader, const ShaderProgramNew& _boxShader, GraphicsTexture _fontTexture) : textShader{_textShader}, boxShader{_boxShader}, fontTexture{_fontTexture} {}
    void init (Renderer* renderer) override {
        textStage = renderer->buildPipelineStage(PipelineStageBuilder(textShader)
                .addVertexAttrib<glm::vec2>(0)
                .addVertexAttrib<glm::vec2>(1)
                .addVertexAttrib<glm::vec3>(2));

        textPosBuffer = renderer->makeBuffer<glm::vec2>(BUFFER_SIZE);
        textUvBuffer = renderer->makeBuffer<glm::vec2>(BUFFER_SIZE);
        textColourBuffer = renderer->makeBuffer<glm::vec3>(BUFFER_SIZE);

        textStage.bindBuffer(0, textPosBuffer);
        textStage.bindBuffer(1, textUvBuffer);
        textStage.bindBuffer(2, textColourBuffer);

        boxStage = renderer->buildPipelineStage(PipelineStageBuilder(boxShader)
                .addVertexAttrib<glm::vec2>(0)
                .addVertexAttrib<glm::vec2>(1)
                .addVertexAttrib<glm::vec4>(2)
                .addVertexAttrib<glm::vec4>(3)
                .addVertexAttrib<glm::vec4>(4));

        boxPosBuffer = renderer->makeBuffer<glm::vec2>(BUFFER_SIZE);
        boxRectPosBuffer = renderer->makeBuffer<glm::vec2>(BUFFER_SIZE);
        boxBorderBuffer = renderer->makeBuffer<glm::vec4>(BUFFER_SIZE);
        boxBgBuffer = renderer->makeBuffer<glm::vec4>(BUFFER_SIZE);
        boxDetailsBuffer = renderer->makeBuffer<glm::vec4>(BUFFER_SIZE);

        boxStage.bindBuffer(0, boxPosBuffer);
        boxStage.bindBuffer(1, boxRectPosBuffer);
        boxStage.bindBuffer(2, boxBorderBuffer);
        boxStage.bindBuffer(3, boxBgBuffer);
        boxStage.bindBuffer(4, boxDetailsBuffer);
    }

    void bufferData () override {
        textStage.bufferAllData();
        boxStage.bufferAllData();
    }

    void render () override {
        fontTexture.bindTexture();
        textStage.render();
        textStage.clearBuffers();

        boxShader.applyUniform<glm::vec2>("screenSize", screenSize);
        boxStage.render();
        boxStage.clearBuffers();
    }

    void bufferText(const RenderedText &text) override {
        textPosBuffer.pushData(text.getPosComp().cbegin(), text.getPosComp().cend());
        textUvBuffer.pushData(text.getUvComp().cbegin(), text.getUvComp().cend());
        textColourBuffer.pushData(text.getColourComp().cbegin(), text.getColourComp().cend());
    }

    void bufferRect(const UIRect &rect) override {
        for (int i = 0; i < 6; i++) {
            int vertexNum = i == 3 ? i : i % 3;

            boxPosBuffer.pushData({rect.screenPos.x + (float)(vertexNum % 2 == 1) * rect.size.x, rect.screenPos.y + (float)(vertexNum / 2 == 1) * rect.size.y});
            boxRectPosBuffer.pushData({-1.0f + (float)(vertexNum % 2 == 1) * 2.0f, -1.0f + (float)(vertexNum / 2 == 1) * 2.0f});
            boxBorderBuffer.pushData(rect.borderColour);
            boxBgBuffer.pushData(rect.bgColour);
            boxDetailsBuffer.pushData(rect.details);
        }
    }

    void setScreenSize(glm::vec2 _screenSize) override {
        screenSize = _screenSize;
    }
};

std::string graphics::UIRenderLayer::getName () {
    return "ui_layer";
}

int graphics::UIRenderLayer::getPriority () {
    return 3;
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
    pipeline = std::make_unique<UIPipelineInt>(textProgram, renderer->getProgramNew("box").orThrow(), fontTexture);
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







