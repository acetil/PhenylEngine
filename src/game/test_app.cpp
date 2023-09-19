#include "common/assets/assets.h"
#include "engine/level/level.h"

#include "test_app.h"
#include "entity/bullet.h"
#include "entity/player.h"
#include "util/debug_console.h"
#include "graphics/ui/ui_manager.h"

game::TestApp::TestApp () = default;

void game::TestApp::init () {
    addBulletSignals(componentManager(), serializer());
    addPlayerComponents(componentManager(), serializer());

    inputSetup(input());

    common::Assets::Load<Level>("resources/maps/testmap")->load();

    graphics::ui::UIButton buttonC{"button"};
    graphics::ui::UIButton buttonC2{"button2"};
    label.text = "Hello World!";

    flexBoxC.add(buttonC.detach());
    flexBoxC.add(buttonC2.detach());
    flexBoxC.add(label);

    uiManager().addUIComp(flexBoxC.detach(), {0, 100});
    uiManager().addUIComp(button4, {500, 300});
    uiManager().addUIComp(button5, {500, 385});
    uiManager().setCurrentTheme(common::Assets::Load<graphics::ui::Theme>("resources/themes/default_theme"));

    stepAction = input().mapInput("debug_step", "key_f7");
    consoleAction = input().mapInput("debug_console", "key_f12");
}

void game::TestApp::fixedUpdate (float deltaTime) {
    playerUpdate(componentManager(), input(), camera());

    if (isStepping) {
        pause();
    }
}

void game::TestApp::update (double deltaTime) {
    if (button4 && !isButtonDown) {
        isButtonDown = true;
        numPresses++;
        label.text = "Pressed " + std::to_string(numPresses) + " times!";
        auto newLabel = graphics::ui::UILabel("label");
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
        util::doDebugConsole(eventBus(), this);
    }
}

void game::TestApp::queueResume () {
    resumeFrames = 2;
}

void game::TestApp::startStepping () {
    isStepping = true;
    pause();
}

void game::TestApp::stopStepping () {
    isStepping = false;
    resume();
}

void game::TestApp::step () {
    if (isStepping) {
        resume();
    }
}
