#pragma once

#include <phenyl/audio.h>
#include <phenyl/application.h>
#include <phenyl/asset.h>
#include <phenyl/input.h>
#include "phenyl/font.h"
#include <phenyl/ui/ui.h>

#include "phenyl/ui/layout.h"
#include "phenyl/ui/widget.h"

namespace test {
    class TestApp : public phenyl::Application2D {
    private:
        // std::vector<phenyl::ui::Label> extraLabels;
        std::vector<phenyl::ui::Widget*> extraWidgets;
        // phenyl::ui::Label label{"label"};
        // phenyl::ui::Flexbox flexBoxC{"flex_box"};

        phenyl::ui::ColumnWidget* column = nullptr;

        phenyl::ui::ButtonWidget* button1 = nullptr;
        phenyl::ui::ButtonWidget* button2 = nullptr;

        phenyl::InputAction stepAction;
        phenyl::InputAction consoleAction;
        phenyl::Asset<phenyl::Font> testFont;

        int numPresses = 0;
        bool isButtonDown = false;
        bool isButtonDown2 = false;
        int resumeFrames = 0;
        bool isStepping = false;
        bool stepDown = false;

        void addLabel ();
        void removeLabel ();
    public:
        TestApp ();

        void init () override;
        void update ();
        void fixedUpdate ();

        void queueResume ();
        void startStepping ();
        void stopStepping ();
        void step ();

        void updateDebugRender (bool doRender);
        void updateProfileRender (bool doRender);
        void dumpLevel (const std::string& path);
    };
}
