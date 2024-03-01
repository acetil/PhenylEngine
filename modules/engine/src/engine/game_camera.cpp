#include "engine/game_camera.h"
#include "graphics/camera.h"
#include "logging/logging.h"


using namespace phenyl::game;

static phenyl::Logger LOGGER{"CAMERA"};

void GameCamera::setPos (glm::vec2 pos) {
    targetPos = pos;
}

void GameCamera::translate (glm::vec2 vec) {
    targetPos += vec;
}

void GameCamera::setScale (float scaleX, float scaleY) {
    targetScale = {scaleX, scaleY};
}

void GameCamera::scaleBy (float scaleX, float scaleY) {
    targetScale.x *= scaleX;
    targetScale.y *= scaleY;
}

void GameCamera::updateCamera (graphics::Camera& camera) {
    PHENYL_LOGT(LOGGER, "Updating camera: pos={}, targetPos={}, scale={}, targetScale={}", currentPos, targetPos, currentScale, targetScale);
    glm::vec2 transVec = targetPos - currentPos;
    camera.translate(transVec.x, transVec.y);
    auto scaleVec = currentScale / targetScale;

    camera.scale(scaleVec.x); // TODO

    currentPos += transVec;
    currentScale = targetScale;
}

glm::vec2 GameCamera::getWorldPos (glm::vec2 screenPos) {
    glm::vec2 worldPos = screenPos / currentScale;
    worldPos += currentPos;
    return worldPos;
}
