#include <phenyl/asset.h>
#include <phenyl/canvas.h>
#include <phenyl/debug.h>
#include <phenyl/level.h>
#include <phenyl/ui/ui.h>

#include "test_app.h"
#include "entity/bullet.h"
#include "entity/player.h"
#include "util/debug_console.h"

static phenyl::Logger LOGGER{"TEST_APP"};

test::TestApp::TestApp () :
    phenyl::Application2D(phenyl::ApplicationProperties{}
        .withResolution(800, 600)
        .withLogFile("debug.log")
        .withRootLogLevel(LEVEL_DEBUG)
        .withWindowTitle("Action Game")
        .withVsync(false)) {}

void test::TestApp::init () {
    InitBullet(this, componentManager());
    InitPlayer(this);

    runtime().addSystem<phenyl::FixedUpdate>("TestApp::fixedUpdate", this, &TestApp::fixedUpdate);
    runtime().addSystem<phenyl::Update>("TestApp::update", this, &TestApp::update);

    auto& input = runtime().resource<phenyl::GameInput>();

    phenyl::Assets::Load<phenyl::Level>("resources/levels/test_level")->load();

    phenyl::ui::Button buttonC{"button"};
    phenyl::ui::Button buttonC2{"button2"};
    label.text = "Hello World!";

    flexBoxC.add(buttonC.detach());
    flexBoxC.add(buttonC2.detach());
    flexBoxC.add(label);

    auto& uiManager = runtime().resource<phenyl::UIManager>();
    uiManager.addUIComp(flexBoxC.detach(), {0, 100});
    uiManager.addUIComp(button4, {500, 300});
    uiManager.addUIComp(button5, {500, 385});

    stepAction = input.addAction("debug_step");
    consoleAction = input.addAction("debug_console");

    input.addActionBinding("debug_step", "keyboard.key_f7");
    input.addActionBinding("debug_console", "keyboard.key_f12");

    testFont = phenyl::Assets::Load<phenyl::graphics::Font>("resources/fonts/OpenSans-Regular");
}

void test::TestApp::fixedUpdate () {
    if (isStepping) {
        pause();
    }
}

void test::TestApp::update () {
    if (button4 && !isButtonDown) {
        isButtonDown = true;
        numPresses++;
        label.text = "Pressed " + std::to_string(numPresses) + " times!";
        auto newLabel = phenyl::ui::Label("label");
        newLabel.text = "Label " + std::to_string(extraLabels.size());
        flexBoxC.add(newLabel);
        extraLabels.emplace_back(std::move(newLabel));
    } else if (!button4 && isButtonDown) {
        isButtonDown = false;
    }

    if (button5 && !isButtonDown2) {
        isButtonDown2 = true;

        if (!extraLabels.empty()) {
            extraLabels.pop_back();
        }
    } else if (!button5 && isButtonDown2) {
        isButtonDown2 = false;
    }

    if (resumeFrames > 0) {
        resumeFrames--;
        if (!resumeFrames) {
            resume();
        }
    }

    if (stepAction.value() && !stepDown) {
        stepDown = true;
        step();
    } else if (!stepAction.value()) {
        stepDown = false;
    }

    if (consoleAction.value()) {
        test::doDebugConsole(this);
    }

    auto& canvas = runtime().resource<phenyl::Canvas>();
    canvas.renderText(glm::vec2{200, 300}, testFont, 11, "Hello World");
}

void test::TestApp::queueResume () {
    resumeFrames = 2;
}

void test::TestApp::startStepping () {
    isStepping = true;
    pause();
}

void test::TestApp::stopStepping () {
    isStepping = false;
    resume();
}

void test::TestApp::step () {
    if (isStepping) {
        resume();
    }
}

void test::TestApp::changeTheme (phenyl::Asset<phenyl::ui::Theme> theme) {
    runtime().resource<phenyl::UIManager>().setCurrentTheme(std::move(theme));
}

void test::TestApp::updateDebugRender (bool doRender) {
    runtime().resource<phenyl::DebugRenderConfig>().doPhysicsRender = doRender;
}

void test::TestApp::updateProfileRender (bool doRender) {
    runtime().resource<phenyl::DebugRenderConfig>().doProfileRender = doRender;
}

void test::TestApp::dumpLevel (const std::string& path) {
    std::ofstream file{path};
    if (file) {
        runtime().resource<phenyl::LevelManager>().dump(file);
    } else {
        PHENYL_LOGE(LOGGER, "Failed to open path \"{}\"!", path);
    }
}
