#include "breakout.h"

#include "ball.h"
#include "controller.h"
#include "paddle.h"
#include "phenyl/font.h"
#include "phenyl/ui/component.h"
#include "phenyl/ui/container.h"
#include "phenyl/ui/layout.h"
#include "phenyl/ui/types.h"
#include "tile.h"
#include "wall.h"

#include <iostream>
#include <phenyl/asset.h>
#include <phenyl/components/physics/2D/collider.h>
#include <phenyl/components/physics/2D/colliders/box_collider.h>
#include <phenyl/debug.h>
#include <phenyl/entrypoint.h>
#include <phenyl/level.h>
#include <phenyl/ui/ui.h>
#include <phenyl/world.h>
#include <utility>

static constexpr std::size_t TileRows = 8;
static constexpr std::size_t TileCols = 7;
static constexpr std::size_t Lives = 4;

static constexpr float TileWidth = 0.12f * 2;
static constexpr float TileHeight = 0.04f * 2;
static constexpr float Padding = 0.01f;

static constexpr float XStart = -1.0f + 0.075f + 0.05 + TileWidth / 2;
static constexpr float YStart = 1.0f - 0.075f - TileHeight / 2;

using namespace breakout;

BreakoutApp::BreakoutApp (phenyl::ApplicationProperties properties) :
    phenyl::Application2D(
        std::move(properties.withResolution(800, 600).withWindowTitle("Breakout!").withVsync(false))) {
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
    // auto& uiManager = runtime().resource<phenyl::UIManager>();
    auto& ui = runtime().resource<phenyl::UI>();
    m_livesLabel = phenyl::ui::Atom<std::string>::Make(std::format("Lives: {}", m_lives));
    ui.root().add<phenyl::ui::DynamicLabel>(phenyl::ui::DynamicLabelProps{
      .text = m_livesLabel,
      .textSize = 14,
      .font = phenyl::Assets::Load<phenyl::Font>("resources/phenyl/fonts/noto-serif"),
      .modifier = phenyl::ui::Modifier{}.withOffset({280, 30}),
    });

    phenyl::Assets::Load<phenyl::Level>("resources/levels/main")->load();

    auto* renderConfig = runtime().resourceMaybe<phenyl::DebugRenderConfig>();
    if (renderConfig) {
        renderConfig->doPhysicsRender = true;
    }
}

void BreakoutApp::addPoints (int points) {
    m_totalPoints += points;

    if (m_totalPoints == TileCols * TileRows * 10) {
        onWin();
    }
}

void BreakoutApp::subtractLife () {
    m_livesLabel.set(std::format("Lives: {}", m_lives - 1));
    if (!--m_lives) {
        runtime().resource<phenyl::UI>().root().add<phenyl::ui::Label>(phenyl::ui::LabelProps{
          .text = "You Lose!",
          .textSize = 72,
          .font = phenyl::Assets::Load<phenyl::Font>("resources/phenyl/fonts/noto-serif"),
          .modifier = phenyl::ui::Modifier{}.withOffset({240, 250}),
        });
        pause();
    }
}

void BreakoutApp::onWin () {
    runtime().resource<phenyl::UI>().root().add<phenyl::ui::Label>(phenyl::ui::LabelProps{
      .text = "You Win!",
      .textSize = 72,
      .font = phenyl::Assets::Load<phenyl::Font>("resources/phenyl/fonts/noto-serif"),
      .modifier = phenyl::ui::Modifier{}.withOffset({240, 250}),
    });
    pause();
}

PHENYL_ENTRYPOINT(BreakoutApp)
