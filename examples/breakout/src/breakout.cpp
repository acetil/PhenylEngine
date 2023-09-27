#include <phenyl/asset.h>
#include <phenyl/component.h>
#include <phenyl/level.h>

#include "breakout.h"
#include "paddle.h"

using namespace breakout;

BreakoutApp::BreakoutApp () {

}

void BreakoutApp::init () {
    //componentManager().addComponent<Paddle>();
    //serializer().addSerializer<Paddle>();
    breakout::initPaddle(componentManager(), serializer(), input());
    phenyl::Assets::Load<phenyl::Level>("resources/levels/main")->load();
    this->setDebugRender(true);
}

void BreakoutApp::update (double deltaTime) {

}

void BreakoutApp::fixedUpdate (float deltaTime) {
    updatePaddle(deltaTime, componentManager(), input(), camera());
}
