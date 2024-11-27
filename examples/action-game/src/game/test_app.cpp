#include <phenyl/asset.h>
#include <phenyl/canvas.h>
#include <phenyl/debug.h>
#include <phenyl/entrypoint.h>
#include <phenyl/level.h>
#include <phenyl/ui/ui.h>

#include "test_app.h"
#include "entity/bullet.h"
#include "entity/player.h"
#include "phenyl/ui/container.h"
#include "phenyl/ui/widget.h"
#include "util/debug_console.h"

static phenyl::Logger LOGGER{"TEST_APP"};

test::TestApp::TestApp (phenyl::ApplicationProperties properties) :
    phenyl::Application2D(properties
        .withResolution(800, 600)
        .withLogFile("debug.log")
        .withRootLogLevel(LEVEL_DEBUG)
        .withWindowTitle("Action Game")
        .withVsync(false)) {}

void test::TestApp::init () {
    InitBullet(this, world());
    InitPlayer(this);

    runtime().addSystem<phenyl::FixedUpdate>("TestApp::fixedUpdate", this, &TestApp::fixedUpdate);
    runtime().addSystem<phenyl::Update>("TestApp::update", this, &TestApp::update);

    auto& input = runtime().resource<phenyl::GameInput>();

    phenyl::Assets::Load<phenyl::Level>("resources/levels/test_level")->load();

    auto& uiManager = runtime().resource<phenyl::UIManager>();
    // uiManager.addUIComp(button4, {500, 300});
    // uiManager.addUIComp(button5, {500, 385});
    button1 = uiManager.root().emplace<phenyl::ui::ButtonWidget>(phenyl::ui::Modifier{}
            .withSize({100, 80})
            .withOffset({500, 300}));
    button1->setDefaultBgColor({1.0, 0.0, 0.0, 1.0});
    button1->setHoverBgColor({0.0, 1.0, 0.0, 1.0});
    button1->setPressBgColor({0.0, 0.0, 1.0, 1.0});
    button1->addListener([&] (const phenyl::ui::ButtonPressEvent&) {
        addLabel();
    });

    button2 = uiManager.root().emplace<phenyl::ui::ButtonWidget>(phenyl::ui::Modifier{}
            .withSize({100, 80})
            .withOffset({500, 385}));
    button2->setDefaultBgColor({1.0, 0.0, 0.0, 1.0});
    button2->setHoverBgColor({0.0, 1.0, 0.0, 1.0});
    button2->setPressBgColor({0.0, 0.0, 1.0, 1.0});
    button2->addListener([&] (const phenyl::ui::ButtonPressEvent&) {
        removeLabel();
    });

    auto* container = uiManager.root().emplace<phenyl::ui::ContainerWidget>(phenyl::ui::Modifier{}.withSize({200, 300}).withOffset({0, 100}));
    container->setBorderColor({1.0f, 1.0f, 1.0f, 1.0f});
    container->setBorderSize(2);
    column = container->emplace<phenyl::ui::ColumnWidget>(phenyl::ui::ColumnDirection::DOWN, phenyl::ui::LayoutArrangement::START, phenyl::ui::LayoutAlignment::START, phenyl::ui::Modifier{});

    /*column = uiManager.root().emplace<phenyl::ui::ColumnWidget>(phenyl::ui::ColumnDirection::DOWN, phenyl::ui::LayoutArrangement::SPACED, phenyl::ui::LayoutAlignment::START, phenyl::ui::Modifier{}
        .withSize({0, 0}, {200, 300})
        .withOffset({200, 100}));*/
    auto* labelWidget = column->emplaceBack<phenyl::ui::LabelWidget>("Hello World 2!");
    labelWidget->setFont(phenyl::Assets::Load<phenyl::Font>("resources/phenyl/fonts/noto-serif"));

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

void test::TestApp::addLabel () {
    numPresses++;
    // label.text = "Pressed " + std::to_string(numPresses) + " times!";
    // auto newLabel = phenyl::ui::Label("label");
    // newLabel.text = "Label " + std::to_string(extraLabels.size());
    // flexBoxC.add(newLabel);
    // extraLabels.emplace_back(std::move(newLabel));

    auto* widget = column->emplaceBack<phenyl::ui::LabelWidget>(std::format("Pressed {} times!", numPresses));
    widget->setFont(phenyl::Assets::Load<phenyl::Font>("resources/phenyl/fonts/noto-serif"));
    extraWidgets.emplace_back(widget);
}

void test::TestApp::removeLabel () {
    // if (!extraLabels.empty()) {
    //     extraLabels.pop_back();
    // }

    if (!extraWidgets.empty()) {
        extraWidgets.back()->queueDestroy();
        extraWidgets.pop_back();
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

PHENYL_ENTRYPOINT(test::TestApp)