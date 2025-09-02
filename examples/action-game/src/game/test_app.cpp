#include "test_app.h"

#include "entity/bullet.h"
#include "entity/player.h"
#include "phenyl/ui/atom.h"
#include "phenyl/ui/component.h"
#include "phenyl/ui/container.h"
#include "phenyl/ui/types.h"
#include "util/debug_console.h"

#include <phenyl/asset.h>
#include <phenyl/canvas.h>
#include <phenyl/debug.h>
#include <phenyl/entrypoint.h>
#include <phenyl/level.h>
#include <phenyl/ui/ui.h>

static phenyl::Logger LOGGER{"TEST_APP"};

struct DynamicListProps {
    phenyl::ui::Atom<std::vector<std::string>> labels;
    phenyl::ui::Modifier modifier;
};

class DynamicList : public phenyl::ui::Component<DynamicList, DynamicListProps> {
public:
    DynamicList (phenyl::UI& ui, DynamicListProps listProps) : UIComponent{ui, std::move(listProps)} {
        useAtom(props().labels);
    }

    phenyl::UIResult render (phenyl::UIContext& ctx) const override {
        std::vector<phenyl::UIFactory> children;
        for (const auto& label : *props().labels) {
            std::string_view labelView = label;
            auto font = phenyl::Assets::Load<phenyl::Font>("resources/phenyl/fonts/noto-serif");
            children.emplace_back([labelView, font] (phenyl::UIContext& ctx) {
                return ctx.render<phenyl::ui::Label>(
                    phenyl::ui::LabelProps{.text = std::string{labelView}, .font = font});
            });
        }

        return ctx.render<phenyl::ui::Container>(phenyl::ui::ContainerProps{
          .borderColor = {1.0f, 1.0f, 1.0f, 1.0f},
          .borderSize = 2.0f,
          .modifier = props().modifier,
          .child =
              [children = std::move(children)] (phenyl::UIContext& ctx) {
                  return ctx.render<phenyl::ui::Column>(phenyl::ui::ColumnProps{
                    .children = children,
                  });
              },
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

    m_testFont = phenyl::Assets::Load<phenyl::graphics::Font>("resources/fonts/OpenSans-Regular");
    updateDebugRender(true);

    auto& ui = runtime().resource<phenyl::UI>();
    m_labelText = phenyl::ui::Atom<std::string>::Make("Hello World Reactive!");
    ui.root().add<phenyl::ui::DynamicLabel>(phenyl::ui::DynamicLabelProps{
      .text = m_labelText,
      .font = phenyl::Assets::Load<phenyl::Font>("resources/phenyl/fonts/noto-serif"),
      .modifier = phenyl::ui::Modifier{}.withOffset({300, 10}),
    });

    m_labels = phenyl::ui::Atom<std::vector<std::string>>::Make("Hello World!");
    ui.root().add<DynamicList>(DynamicListProps{
      .labels = m_labels,
      .modifier = phenyl::ui::Modifier{}.withOffset({0, 100}).withSize({200, 300}),
    });

    ui.root().add<phenyl::ui::Button>(phenyl::ui::ButtonProps{
      .defaultBgColor = {1.0, 0.0, 0.0, 1.0},
      .hoverBgColor = glm::vec4{0.0, 1.0, 0.0, 1.0},
      .pressBgColor = glm::vec4{0.0, 0.0, 1.0, 1.0},
      .modifier = phenyl::ui::Modifier{}.withSize({100, 80}).withOffset({500, 300}),
      .onPress = [&] () { addLabel(); },
    });

    ui.root().add<phenyl::ui::Button>(phenyl::ui::ButtonProps{
      .defaultBgColor = {1.0, 0.0, 0.0, 1.0},
      .hoverBgColor = glm::vec4{0.0, 1.0, 0.0, 1.0},
      .pressBgColor = glm::vec4{0.0, 0.0, 1.0, 1.0},
      .modifier = phenyl::ui::Modifier{}.withSize({100, 80}).withOffset({500, 385}),
      .onPress = [&] () { removeLabel(); },
    });
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

    m_labels.update([&] (auto& labels) { labels.emplace_back(std::format("Pressed {} times!", m_numPresses)); });
    m_labelText.set(std::format("Hello World Reactive! {}", m_numPresses));
}

void test::TestApp::removeLabel () {
    if (!m_labels->empty()) {
        m_labels.update([] (auto& labels) { labels.pop_back(); });
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
