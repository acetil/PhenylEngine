#include <iostream>
#include <utility>

#include <phenyl/asset.h>
#include <phenyl/component.h>
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

BreakoutApp::BreakoutApp (phenyl::ApplicationProperties properties) : phenyl::Application2D(std::move(properties.withLogFile("debug.log")
        .withRootLogLevel(LEVEL_DEBUG)
        .withResolution(800, 600)
        .withWindowTitle("Breakout!")
        .withVsync(false))) {
    lives = Lives;
}

void BreakoutApp::init () {
    breakout::InitPaddle(this, runtime());
    breakout::InitBall(this, world());
    breakout::InitTile(this, world());
    breakout::InitWall(this, world());
    TileController::Init(this, runtime());

    //tilePrefab = phenyl::Assets::Load<phenyl::Prefab>("resources/prefabs/tile");

    //pointsLabel.text = "Points: 0";
    //livesLabel.text = "Lives: " + std::to_string(lives);

    auto& uiManager = runtime().resource<phenyl::UIManager>();

    //uiManager.addUIComp(pointsLabel, {180, 30});
    //uiManager.addUIComp(livesLabel, {280, 30});
    //auto* textBox = uiManager.root().emplace<phenyl::ui::ColumnWidget>(phenyl::ui::Modifier{}.withOffset({180, 30}));
    //pointsLabel = textBox->emplaceBack<phenyl::ui::LabelWidget>("Points: 0", 14, phenyl::Assets::Load<phenyl::Font>("resources/phenyl/fonts/noto-serif"));
    livesLabel = uiManager.root().emplace<phenyl::ui::LabelWidget>(std::format("Lives: {}", lives), 14, phenyl::Assets::Load<phenyl::Font>("resources/phenyl/fonts/noto-serif"), phenyl::ui::Modifier{}.withOffset({280, 30}));

    phenyl::Assets::Load<phenyl::Level>("resources/levels/main")->load();

    /*for (std::size_t i = 0; i < TileRows; i++) {
        for (std::size_t j = 0; j < TileCols; j++) {
            auto tileEntity = world().create();
            tilePrefab->instantiate(tileEntity);
            tileEntity.apply<phenyl::GlobalTransform2D>([i, j] (phenyl::GlobalTransform2D& transform) {
                transform.transform2D.setPosition(glm::vec2{XStart + (float)j * (TileWidth + Padding), YStart - (float)i * (TileHeight + Padding)});
            });
        }
    }*/

    auto* renderConfig = runtime().resourceMaybe<phenyl::DebugRenderConfig>();
    if (renderConfig) {
        renderConfig->doPhysicsRender = true;
    }
}

void BreakoutApp::addPoints (int points) {
    totalPoints += points;
    //pointsLabel.text = "Points: " + std::to_string(totalPoints);
    //pointsLabel->setText(std::format("Points: {}", totalPoints));

    if (totalPoints == TileCols * TileRows * 10) {
        runtime().resource<phenyl::UIManager>().root().emplace<phenyl::ui::ContainerWidget>(phenyl::ui::LabelWidget{"You Win!", 72, phenyl::Assets::Load<phenyl::Font>("resources/phenyl/fonts/noto-serif")}, phenyl::ui::Modifier{}.withOffset({240, 250}));
        pause();
    }
}

void BreakoutApp::subtractLife () {
    //livesLabel.text = "Lives: " + std::to_string(lives - 1);
    livesLabel->setText(std::format("Lives: {}", lives - 1));
    //livesLabel->setText("Lives: " + std::to_string(lives - 1));
    if (!--lives) {
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