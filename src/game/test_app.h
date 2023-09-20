#pragma once

#include "engine/application.h"
#include "graphics/ui/components/ui_button.h"
#include "graphics/ui/components/ui_flexbox.h"
#include "graphics/ui/components/ui_label.h"
#include "engine/input/game_input.h"
#include "common/assets/asset.h"

namespace game {
    class TestApp : public engine::Application {
    private:
        std::vector<graphics::ui::UILabel> extraLabels;
        graphics::ui::UILabel label{"label"};
        graphics::ui::UIFlexbox flexBoxC{"flex_box"};

        graphics::ui::UIButton button4{"button"};
        graphics::ui::UIButton button5{"button"};

        common::InputAction stepAction;
        common::InputAction consoleAction;

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

        void changeTheme (common::Asset<graphics::ui::Theme> theme);
        void updateDebugRender (bool doRender);
        void updateProfileRender (bool doRender);
        void dumpLevel (const std::string& path);
    };
}