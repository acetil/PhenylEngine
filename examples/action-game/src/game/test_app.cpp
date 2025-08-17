#include "test_app.h"

#include "entity/bullet.h"
#include "entity/player.h"
#include "phenyl/ui/atom.h"
#include "phenyl/ui/component.h"
#include "phenyl/ui/container.h"
#include "phenyl/ui/widget.h"
#include "ui/reactive/root.h"
#include "util/debug_console.h"

#include <phenyl/asset.h>
#include <phenyl/canvas.h>
#include <phenyl/debug.h>
#include <phenyl/entrypoint.h>
#include <phenyl/level.h>
#include <phenyl/ui/ui.h>

static phenyl::Logger LOGGER{"TEST_APP"};

struct DynamicLabelProps {
    phenyl::ui::Atom<std::string> text;
    phenyl::ui::Modifier modifier;
};

class DynamicLabel : public phenyl::ui::Component<DynamicLabel, DynamicLabelProps> {
public:
    DynamicLabel (phenyl::UI& ui, DynamicLabelProps labelProps) : UIComponent{ui, std::move(labelProps)} {
        useAtom(props().text);
    }

    void render (phenyl::UI& ui) override {
        ui.render<phenyl::ui::Label>(phenyl::ui::LabelProps{
          .text = *props().text,
          .font = phenyl::Assets::Load<phenyl::Font>("resources/phenyl/fonts/noto-serif"),
          .modifier = props().modifier,
        });
    }
};

test::TestApp::TestApp (phenyl::ApplicationProperties properties) :
    phenyl::Application2D(properties.withResolution(800, 600).withWindowTitle("Action Game").withVsync(false)) {}

void test::TestApp::init () {
    InitBullet(this, world());
    InitPlayer(this);

    auto& input = runtime().resource<phenyl::GameInput>();
    m_stepAction = input.addAction("debug_step");
    m_consoleAction = input.addAction("debug_console");

    input.addActionBinding("debug_step", "keyboard.key_f7");
    input.addActionBinding("debug_console", "keyboard.key_f12");

    runtime().addSystem<phenyl::FixedUpdate>("TestApp::fixedUpdate", this, &TestApp::fixedUpdate);
    runtime().addSystem<phenyl::Update>("TestApp::update", this, &TestApp::update);
}

void test::TestApp::postInit () {
    phenyl::Assets::Load<phenyl::Level>("resources/levels/test_level")->load();

    auto& uiManager = runtime().resource<phenyl::UIManager>();
    // uiManager.addUIComp(button4, {500, 300});
    // uiManager.addUIComp(button5, {500, 385});
    m_button1 = uiManager.root().emplace<phenyl::ui::ButtonWidget>(
        phenyl::ui::Modifier{}.withSize({100, 80}).withOffset({500, 300}));
    m_button1->setDefaultBgColor({1.0, 0.0, 0.0, 1.0});
    m_button1->setHoverBgColor({0.0, 1.0, 0.0, 1.0});
    m_button1->setPressBgColor({0.0, 0.0, 1.0, 1.0});
    m_button1->addListener([&] (const phenyl::ui::ButtonPressEvent&) { addLabel(); });

    m_button2 = uiManager.root().emplace<phenyl::ui::ButtonWidget>(
        phenyl::ui::Modifier{}.withSize({100, 80}).withOffset({500, 385}));
    m_button2->setDefaultBgColor({1.0, 0.0, 0.0, 1.0});
    m_button2->setHoverBgColor({0.0, 1.0, 0.0, 1.0});
    m_button2->setPressBgColor({0.0, 0.0, 1.0, 1.0});
    m_button2->addListener([&] (const phenyl::ui::ButtonPressEvent&) { removeLabel(); });

    auto* container = uiManager.root().emplace<phenyl::ui::ContainerWidget>(
        phenyl::ui::Modifier{}.withSize({200, 300}).withOffset({0, 100}));
    container->setBorderColor({1.0f, 1.0f, 1.0f, 1.0f});
    container->setBorderSize(2);
    m_column = container->emplace<phenyl::ui::ColumnWidget>(phenyl::ui::ColumnDirection::DOWN,
        phenyl::ui::LayoutArrangement::START, phenyl::ui::LayoutAlignment::START, phenyl::ui::Modifier{});

    /*column =
       uiManager.root().emplace<phenyl::ui::ColumnWidget>(phenyl::ui::ColumnDirection::DOWN,
       phenyl::ui::LayoutArrangement::SPACED, phenyl::ui::LayoutAlignment::START,
       phenyl::ui::Modifier{} .withSize({0, 0}, {200, 300}) .withOffset({200, 100}));*/
    auto* labelWidget = m_column->emplaceBack<phenyl::ui::LabelWidget>("Hello World 2!");
    labelWidget->setFont(phenyl::Assets::Load<phenyl::Font>("resources/phenyl/fonts/noto-serif"));

    m_testFont = phenyl::Assets::Load<phenyl::graphics::Font>("resources/fonts/OpenSans-Regular");
    updateDebugRender(true);

    auto& ui = runtime().resource<phenyl::UI>();
    m_labelText = phenyl::ui::Atom<std::string>::Make("Hello World Reactive!");
    ui.root().add<DynamicLabel>(
        DynamicLabelProps{.text = m_labelText, .modifier = phenyl::ui::Modifier{}.withOffset({300, 10})});
}

void test::TestApp::fixedUpdate () {
    if (m_isStepping) {
        pause();
    }
}

void test::TestApp::update () {
    if (m_resumeFrames > 0) {
        m_resumeFrames--;
        if (!m_resumeFrames) {
            resume();
        }
    }

    if (m_stepAction.value() && !m_stepDown) {
        m_stepDown = true;
        step();
    } else if (!m_stepAction.value()) {
        m_stepDown = false;
    }

    if (m_consoleAction.value()) {
        test::DoDebugConsole(this);
    }

    auto& canvas = runtime().resource<phenyl::Canvas>();
    canvas.renderText(glm::vec2{200, 300}, m_testFont, 11, "Hello World");
}

void test::TestApp::addLabel () {
    m_numPresses++;
    // label.text = "Pressed " + std::to_string(numPresses) + " times!";
    // auto newLabel = phenyl::ui::Label("label");
    // newLabel.text = "Label " + std::to_string(extraLabels.size());
    // flexBoxC.add(newLabel);
    // extraLabels.emplace_back(std::move(newLabel));

    auto* widget = m_column->emplaceBack<phenyl::ui::LabelWidget>(std::format("Pressed {} times!", m_numPresses));
    widget->setFont(phenyl::Assets::Load<phenyl::Font>("resources/phenyl/fonts/noto-serif"));
    m_extraWidgets.emplace_back(widget);

    m_labelText.set(std::format("Hello World Reactive! {}", m_numPresses));
}

void test::TestApp::removeLabel () {
    // if (!extraLabels.empty()) {
    //     extraLabels.pop_back();
    // }

    if (!m_extraWidgets.empty()) {
        m_extraWidgets.back()->queueDestroy();
        m_extraWidgets.pop_back();
    }
}

void test::TestApp::queueResume () {
    m_resumeFrames = 2;
}

void test::TestApp::startStepping () {
    m_isStepping = true;
    pause();
}

void test::TestApp::stopStepping () {
    m_isStepping = false;
    resume();
}

void test::TestApp::step () {
    if (m_isStepping) {
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
