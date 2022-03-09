#include "graphics/renderlayer/ui_layer.h"

#include "graphics/font/font.h"

#include "logging/logging.h"

using namespace graphics;

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

}

int graphics::UIRenderLayer::getUniformId (std::string uniformName) {
    return 0;
}

void graphics::UIRenderLayer::applyUniform (int uniformId, void* data) {

}

void graphics::UIRenderLayer::applyCamera (graphics::Camera camera) {

}

void graphics::UIRenderLayer::render (graphics::Renderer* renderer, graphics::FrameBuffer* frameBuf) {
    textProgram.bind();

    fontTexture.bindTexture();

    int size = textBuffer->currentSize() / 3 / 4 / 2;

    renderer->bufferData(textIds, textBuffer);

    renderer->render(textIds, textProgram, size);
}

void graphics::UIRenderLayer::bufferStr (graphics::Font& font, const std::string& text, int size, int x, int y, glm::vec3 colour) {
    font.renderText(text, size, x, y, colour, textBuffer);
}

UIRenderLayer::UIRenderLayer (GraphicsTexture _fontTexture, Renderer* renderer) : fontTexture(_fontTexture),
                                                                                  textProgram(renderer->getProgramNew(
                                                                                          "text").orThrow()) {
    textIds = renderer->getBufferIds(3, 200 * 12 * sizeof(float), {2, 2, 3});
    textBuffer[0] = Buffer(200 * 12, sizeof(float), false);
    textBuffer[1] = Buffer(200 * 12, sizeof(float), false);
    textBuffer[2] = Buffer(200 * 18, sizeof(float), false);
}







