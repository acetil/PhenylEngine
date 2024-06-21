#include <iostream>
#include <utility>

#include <phenyl/asset.h>
#include <phenyl/component.h>
#include <phenyl/components/physics/2D/colliders/box_collider.h>
#include <phenyl/components/physics/2D/collider.h>
#include <phenyl/debug.h>
#include <phenyl/level.h>
#include <phenyl/ui/ui.h>

#include "ball.h"
#include "breakout.h"
#include "paddle.h"
#include "tile.h"
#include "wall.h"

static constexpr std::size_t TileRows = 8;
static constexpr std::size_t TileCols = 7;
static constexpr std::size_t Lives = 4;

static constexpr float TileWidth = 0.12f * 2;
static constexpr float TileHeight = 0.04f * 2;
static constexpr float Padding = 0.01f;

static constexpr float XStart = -1.0f + 0.075f  + 0.05 + TileWidth / 2;
static constexpr float YStart = 1.0f - 0.075f - TileHeight / 2;

using namespace breakout;

BreakoutApp::BreakoutApp (phenyl::ApplicationProperties properties) : phenyl::Application2D(std::move(properties)), pointsLabel{"label"}, livesLabel{"label"} {
    lives = Lives;
}

void BreakoutApp::init () {
    //componentManager().addComponent<Paddle>();
    //serializer().addSerializer<Paddle>();
    breakout::InitPaddle(this, runtime());
    breakout::InitBall(this, componentManager());
    breakout::InitTile(this, componentManager());
    breakout::InitWall(this, componentManager());

    tilePrefab = phenyl::Assets::Load<phenyl::Prefab>("resources/prefabs/tile");

    pointsLabel.text = "Points: 0";
    livesLabel.text = "Lives: " + std::to_string(lives);

    auto& uiManager = runtime().resource<phenyl::UIManager>();

    uiManager.addUIComp(pointsLabel, {180, 30});
    uiManager.addUIComp(livesLabel, {280, 30});

    phenyl::Assets::Load<phenyl::Level>("resources/levels/main")->load();

    for (std::size_t i = 0; i < TileRows; i++) {
        for (std::size_t j = 0; j < TileCols; j++) {
            tilePrefab->instantiate()
                .complete()
                .apply<phenyl::GlobalTransform2D>([i, j] (phenyl::GlobalTransform2D& transform) {
                    transform.transform2D.setPosition(glm::vec2{XStart + (float)j * (TileWidth + Padding), YStart - (float)i * (TileHeight + Padding)});
                });
        }
    }

    auto* renderConfig = runtime().resourceMaybe<phenyl::DebugRenderConfig>();
    if (renderConfig) {
        renderConfig->doPhysicsRender = true;
    }
}

void BreakoutApp::update (double deltaTime) {

}

void BreakoutApp::fixedUpdate (double deltaTime) {
    //updatePaddle((float)deltaTime, runtime());
    //updateBall(componentManager());
}

void BreakoutApp::addPoints (int points) {
    totalPoints += points;
    pointsLabel.text = "Points: " + std::to_string(totalPoints);

    if (totalPoints == TileCols * TileRows * 10) {
        phenyl::ui::Label winLabel{"big_label"};
        winLabel.text = "You Win!";
        runtime().resource<phenyl::UIManager>().addUIComp(winLabel.detach(), {240, 250});
        pause();
    }
}

void BreakoutApp::subtractLife () {
    livesLabel.text = "Lives: " + std::to_string(lives - 1);
    if (!--lives) {
        phenyl::ui::Label loseLabel{"big_label"};
        loseLabel.text = "You Lose!";
        runtime().resource<phenyl::UIManager>().addUIComp(loseLabel.detach(), {240, 250});
        pause();
    }
}
