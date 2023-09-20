#pragma once

#include "engine/application.h"
#include "graphics/ui/components/ui_button.h"
#include "graphics/ui/components/ui_flexbox.h"
#include "graphics/ui/components/ui_label.h"
#include "engine/input/game_input.h"
#include "common/assets/asset.h"

namespace test {
    class TestApp : public phenyl::engine::Application {
    private:
        std::vector<phenyl::graphics::ui::UILabel> extraLabels;
        phenyl::graphics::ui::UILabel label{"label"};
        phenyl::graphics::ui::UIFlexbox flexBoxC{"flex_box"};

        phenyl::graphics::ui::UIButton button4{"button"};
        phenyl::graphics::ui::UIButton button5{"button"};

        phenyl::common::InputAction stepAction;
        phenyl::common::InputAction consoleAction;

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

        void changeTheme (phenyl::common::Asset<phenyl::graphics::ui::Theme> theme);
        void updateDebugRender (bool doRender);
        void updateProfileRender (bool doRender);
        void dumpLevel (const std::string& path);
    };
}