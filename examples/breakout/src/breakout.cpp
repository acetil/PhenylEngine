#include <iostream>

#include <phenyl/asset.h>
#include <phenyl/component.h>
#include <phenyl/components/physics/2D/colliders/box_collider.h>
#include <phenyl/components/physics/2D/collider.h>
#include <phenyl/level.h>

#include "ball.h"
#include "breakout.h"
#include "paddle.h"
#include "tile.h"

using namespace breakout;

BreakoutApp::BreakoutApp () : pointsLabel{"label"} {}

void BreakoutApp::init () {
    //componentManager().addComponent<Paddle>();
    //serializer().addSerializer<Paddle>();
    breakout::initPaddle(componentManager(), serializer(), input());
    breakout::initBall(componentManager(), serializer());
    breakout::initTile(componentManager(), serializer(), this);

    pointsLabel.text = "Points: 0";

    uiManager().addUIComp(pointsLabel, {180, 20});

    phenyl::Assets::Load<phenyl::Level>("resources/levels/main")->load();
    this->setDebugRender(true);
}

void BreakoutApp::update (double deltaTime) {

}

void BreakoutApp::fixedUpdate (float deltaTime) {
    updatePaddle(deltaTime, componentManager(), input(), camera());
}

void BreakoutApp::addPoints (int points) {
    totalPoints += points;
    pointsLabel.text = "Points: " + std::to_string(totalPoints);
}
