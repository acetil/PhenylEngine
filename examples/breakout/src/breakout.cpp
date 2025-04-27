#include <iostream>
#include <utility>

#include <phenyl/asset.h>
#include <phenyl/world.h>
#include <phenyl/components/physics/2D/colliders/box_collider.h>
#include <phenyl/components/physics/2D/collider.h>
#include <phenyl/debug.h>
#include <phenyl/entrypoint.h>
#include <phenyl/level.h>
#include <phenyl/ui/ui.h>

#include "ball.h"
#include "breakout.h"

#include "controller.h"
#include "paddle.h"
#include "tile.h"
#include "wall.h"
#include "phenyl/font.h"
#include "phenyl/ui/container.h"
#include "phenyl/ui/layout.h"
#include "phenyl/ui/widget.h"

static constexpr std::size_t TileRows = 8;
static constexpr std::size_t TileCols = 7;
static constexpr std::size_t Lives = 4;

static constexpr float TileWidth = 0.12f * 2;
static constexpr float TileHeight = 0.04f * 2;
static constexpr float Padding = 0.01f;

static constexpr float XStart = -1.0f + 0.075f  + 0.05 + TileWidth / 2;
static constexpr float YStart = 1.0f - 0.075f - TileHeight / 2;

using namespace breakout;

BreakoutApp::BreakoutApp (phenyl::ApplicationProperties properties) : phenyl::Application2D(std::move(properties
        .withResolution(800, 600)
        .withWindowTitle("Breakout!")
        .withVsync(false))) {
    m_lives = Lives;
}

void BreakoutApp::init () {
    breakout::InitPaddle(this, runtime());
    breakout::InitBall(this, world());
    breakout::InitTile(this, world());
    breakout::InitWall(this, world());
    TileController::Init(this, runtime());
}
void BreakoutApp::postInit () {
    auto& uiManager = runtime().resource<phenyl::UIManager>();

    m_livesLabel = uiManager.root().emplace<phenyl::ui::LabelWidget>(std::format("Lives: {}", m_lives), 14,
        phenyl::Assets::Load<phenyl::Font>("resources/phenyl/fonts/noto-serif"), phenyl::ui::Modifier{}.withOffset({280, 30}));

    phenyl::Assets::Load<phenyl::Level>("resources/levels/main")->load();

    auto* renderConfig = runtime().resourceMaybe<phenyl::DebugRenderConfig>();
    if (renderConfig) {
        renderConfig->doPhysicsRender = true;
    }
}


void BreakoutApp::addPoints (int points) {
    m_totalPoints += points;
    //pointsLabel.text = "Points: " + std::to_string(totalPoints);
    //pointsLabel->setText(std::format("Points: {}", totalPoints));

    if (m_totalPoints == TileCols * TileRows * 10) {
        runtime().resource<phenyl::UIManager>().root().emplace<phenyl::ui::ContainerWidget>(phenyl::ui::LabelWidget{"You Win!", 72, phenyl::Assets::Load<phenyl::Font>("resources/phenyl/fonts/noto-serif")}, phenyl::ui::Modifier{}.withOffset({240, 250}));
        pause();
    }
}

void BreakoutApp::subtractLife () {
    //livesLabel.text = "Lives: " + std::to_string(lives - 1);
    m_livesLabel->setText(std::format("Lives: {}", m_lives - 1));
    //livesLabel->setText("Lives: " + std::to_string(lives - 1));
    if (!--m_lives) {
        runtime().resource<phenyl::UIManager>().root().emplace<phenyl::ui::ContainerWidget>(phenyl::ui::LabelWidget{"You Lose!", 72, phenyl::Assets::Load<phenyl::Font>("resources/phenyl/fonts/noto-serif")}, phenyl::ui::Modifier{}.withOffset({240, 250}));
        pause();
    }
}

void BreakoutApp::onWin () {
    // phenyl::ui::Label winLabel{"big_label"};
    // winLabel.text = "You Win!";
    // runtime().resource<phenyl::UIManager>().addUIComp(winLabel.detach(), {240, 250});
    runtime().resource<phenyl::UIManager>().root().emplace<phenyl::ui::ContainerWidget>(phenyl::ui::LabelWidget{"You Win!", 72, phenyl::Assets::Load<phenyl::Font>("resources/phenyl/fonts/noto-serif")}, phenyl::ui::Modifier{}.withOffset({240, 250}));
    pause();
}

PHENYL_ENTRYPOINT(BreakoutApp)