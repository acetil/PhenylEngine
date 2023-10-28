module;

#include <string>
#include <vector>

#include <phenyl/asset.h>
#include <phenyl/level.h>
#include <phenyl/ui/ui.h>
//#include <phenyl/audio.h>
#include <phenyl/application.h>
//#include <phenyl/component.h>
//#include <phenyl/engine.h>
#include <phenyl/input.h>

export module test_app;

import player;
import bullet;
import debug_console;

namespace test {
    export class TestApp;

    //void doDebugConsole (test::TestApp* app);

    export class TestApp : public phenyl::Application {
    private:
        std::vector<phenyl::ui::Label> extraLabels;
        phenyl::ui::Label label{"label"};
        phenyl::ui::Flexbox flexBoxC{"flex_box"};

        phenyl::ui::Button button4{"button"};
        phenyl::ui::Button button5{"button"};

        phenyl::InputAction stepAction;
        phenyl::InputAction consoleAction;

        int numPresses = 0;
        bool isButtonDown = false;
        bool isButtonDown2 = false;
        int resumeFrames = 0;
        bool isStepping = false;
        bool stepDown = false;
    public:
        TestApp () : phenyl::Application(phenyl::ApplicationProperties{}
            .withResolution(800, 600)
            .withWindowTitle("Action Game")
            .withVsync(false)) {}

        void init () override {
            addBulletSignals(this, componentManager());
            addPlayerComponents(this);

            inputSetup(input());

            phenyl::Assets::Load<phenyl::Level>("resources/levels/test_level")->load();

            phenyl::ui::Button buttonC{"button"};
            phenyl::ui::Button buttonC2{"button2"};
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

        void update (double deltaTime) override {
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

        void fixedUpdate (float deltaTime) override {
            playerUpdate(componentManager(), input(), camera());

            if (isStepping) {
                pause();
            }
        }

        void queueResume () {
            resumeFrames = 2;
        }

        void startStepping () {
            isStepping = true;
            pause();
        }

        void stopStepping () {
            isStepping = false;
            resume();
        }

        void step () {
            if (isStepping) {
                resume();
            }
        }

        void changeTheme (phenyl::Asset<phenyl::ui::Theme> theme) {
            uiManager().setCurrentTheme(std::move(theme));
        }

        void updateDebugRender (bool doRender) {
            setDebugRender(doRender);
        }

        void updateProfileRender (bool doRender) {
            setProfileRender(doRender);
        }

        void dumpLevel (const std::string& path) {
            std::ofstream file{path};
            if (file) {
                phenyl::Application::dumpLevel(file);
            } else {
                phenyl::game::logging::log(LEVEL_ERROR, "Failed to open path \"{}\"!", path);
            }
        }
    };
}