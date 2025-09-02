#pragma once

#include "phenyl/font.h"
#include "phenyl/ui/atom.h"

#include <phenyl/application.h>
#include <phenyl/asset.h>
#include <phenyl/audio.h>
#include <phenyl/input.h>
#include <phenyl/ui/layout.h>
#include <phenyl/ui/types.h>
#include <phenyl/ui/ui.h>

namespace test {
class TestApp : public phenyl::Application2D {
public:
    TestApp (phenyl::ApplicationProperties properties);

    void init () override;
    void postInit () override;
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
    phenyl::InputAction m_stepAction;
    phenyl::InputAction m_consoleAction;
    std::shared_ptr<phenyl::Font> m_testFont;

    phenyl::ui::Atom<std::string> m_labelText;
    phenyl::ui::Atom<std::vector<std::string>> m_labels;

    int m_numPresses = 0;
    bool m_isButtonDown = false;
    bool m_isButtonDown2 = false;
    int m_resumeFrames = 0;
    bool m_isStepping = false;
    bool m_stepDown = false;

    void addLabel ();
    void removeLabel ();
};
} // namespace test
