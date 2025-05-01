#pragma once

#include <phenyl/audio.h>
#include <phenyl/application.h>
#include <phenyl/asset.h>
#include <phenyl/input.h>
#include "phenyl/font.h"
#include <phenyl/ui/ui.h>

#include <phenyl/ui/layout.h>
#include <phenyl/ui/widget.h>

namespace test {
    class TestApp : public phenyl::Application2D {
    public:
        TestApp (phenyl::ApplicationProperties properties);

        void init () override;
        void postInit() override;
        void update ();
        void fixedUpdate ();

        void queueResume ();
        void startStepping ();
        void stopStepping ();
        void step ();

        void updateDebugRender (bool doRender);
        void updateProfileRender (bool doRender);
        void dumpLevel (const std::string& path);

    private:
        // std::vector<phenyl::ui::Label> extraLabels;
        std::vector<phenyl::ui::Widget*> m_extraWidgets;
        // phenyl::ui::Label label{"label"};
        // phenyl::ui::Flexbox flexBoxC{"flex_box"};

        phenyl::ui::ColumnWidget* m_column = nullptr;

        phenyl::ui::ButtonWidget* m_button1 = nullptr;
        phenyl::ui::ButtonWidget* m_button2 = nullptr;

        phenyl::InputAction m_stepAction;
        phenyl::InputAction m_consoleAction;
        phenyl::Asset<phenyl::Font> m_testFont;

        int m_numPresses = 0;
        bool m_isButtonDown = false;
        bool m_isButtonDown2 = false;
        int m_resumeFrames = 0;
        bool m_isStepping = false;
        bool m_stepDown = false;

        void addLabel ();
        void removeLabel ();
    };
}
