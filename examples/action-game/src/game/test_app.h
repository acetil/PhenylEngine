#pragma once

#include <phenyl/application.h>
#include <phenyl/asset.h>
#include <phenyl/input.h>
#include <phenyl/ui/ui.h>

namespace test {
    class TestApp : public phenyl::Application {
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
        TestApp ();

        void init () override;
        void update (double deltaTime) override;
        void fixedUpdate (float deltaTime) override;

        void queueResume ();
        void startStepping ();
        void stopStepping ();
        void step ();

        void changeTheme (phenyl::Asset<phenyl::ui::Theme> theme);
        void updateDebugRender (bool doRender);
        void updateProfileRender (bool doRender);
        void dumpLevel (const std::string& path);
    };
}