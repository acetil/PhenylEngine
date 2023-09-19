#pragma once

#include "engine/application.h"
#include "graphics/ui/components/ui_button.h"
#include "graphics/ui/components/ui_flexbox.h"
#include "graphics/ui/components/ui_label.h"
#include "event/event_scope.h"

namespace game {
    class TestApp : public engine::Application {
    private:
        std::vector<graphics::ui::UILabel> extraLabels;
        graphics::ui::UILabel label{"label"};
        graphics::ui::UIFlexbox flexBoxC{"flex_box"};

        graphics::ui::UIButton button4{"button"};
        graphics::ui::UIButton button5{"button"};

        event::EventScope scope;

        int numPresses = 0;
        bool isButtonDown = false;
        bool isButtonDown2 = false;
        int resumeFrames = 0;
        bool isStepping = false;
    public:
        TestApp ();

        void init () override;
        void update (double deltaTime) override;
        void fixedUpdate (float deltaTime) override;

        void queueResume ();
    };
}