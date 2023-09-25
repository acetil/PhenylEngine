#include "common/assets/assets.h"
#include "engine/level/level.h"

#include "test_app.h"
#include "entity/bullet.h"
#include "entity/player.h"
#include "util/debug_console.h"
#include "graphics/ui/ui_manager.h"

test::TestApp::TestApp () = default;

void test::TestApp::init () {
    addBulletSignals(componentManager(), serializer());
    addPlayerComponents(componentManager(), serializer());

    inputSetup(input());

    phenyl::common::Assets::Load<phenyl::game::Level>("resources/maps/testmap")->load();

    phenyl::graphics::ui::UIButton buttonC{"button"};
    phenyl::graphics::ui::UIButton buttonC2{"button2"};
    label.text = "Hello World!";

    flexBoxC.add(buttonC.detach());
    flexBoxC.add(buttonC2.detach());
    flexBoxC.add(label);

    uiManager().addUIComp(flexBoxC.detach(), {0, 100});
    uiManager().addUIComp(button4, {500, 300});
    uiManager().addUIComp(button5, {500, 385});

    stepAction = input().mapInput("debug_step", "key_f7");
    consoleAction = input().mapInput("debug_console", "key_f12");
}

void test::TestApp::fixedUpdate (float deltaTime) {
    playerUpdate(componentManager(), input(), camera());

    if (isStepping) {
        pause();
    }
}

void test::TestApp::update (double deltaTime) {
    if (button4 && !isButtonDown) {
        isButtonDown = true;
        numPresses++;
        label.text = "Pressed " + std::to_string(numPresses) + " times!";
        auto newLabel = phenyl::graphics::ui::UILabel("label");
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

    if (input().isDown(stepAction) && !stepDown) {
        stepDown = true;
        step();
    } else if (!input().isDown(stepAction)) {
        stepDown = false;
    }

    if (input().isDown(consoleAction)) {
        test::doDebugConsole(this);
    }
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

void test::TestApp::changeTheme (phenyl::common::Asset<phenyl::graphics::ui::Theme> theme) {
    uiManager().setCurrentTheme(std::move(theme));
}

void test::TestApp::updateDebugRender (bool doRender) {
    setDebugRender(doRender);
}

void test::TestApp::updateProfileRender (bool doRender) {
    setProfileRender(doRender);
}

void test::TestApp::dumpLevel (const std::string& path) {
    std::ofstream file{path};
    if (file) {
        phenyl::engine::Application::dumpLevel(file);
    } else {
        phenyl::game::logging::log(LEVEL_ERROR, "Failed to open path \"{}\"!", path);
    }
}
