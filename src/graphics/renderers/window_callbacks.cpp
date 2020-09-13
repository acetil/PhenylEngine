#include "window_callbacks.h"
#include "graphics/maths_headers.h"
#include "event/events/cursor_position_change.h"
using namespace graphics;

void graphics::onMousePosChange (WindowCallbackContext* ctx, double windowX, double windowY) {
    auto cam = ctx->graphics->getCamera();
    glm::vec2 realPos = {(float)(windowX * 2 - 1), (float)(windowY * 2 - 1)};
    auto invMat = glm::inverse(cam.getCamMatrix());
    glm::vec4 worldPos4 = invMat * glm::vec4(realPos, 0, 1);
    //ctx->eventBus->raiseEvent(event::CursorPosChangeEvent(glm::vec2((float)windowX, (float)windowY),
                                                          //glm::vec2(worldPos4.x, worldPos4.y)));
}