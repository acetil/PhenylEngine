#pragma once

#include <phenyl/application.h>
#include <phenyl/ui/label.h>

namespace breakout {
    class BreakoutApp : public phenyl::Application {
    private:
        int totalPoints = 0;
        phenyl::ui::Label pointsLabel;

        void init () override;

        void update (double deltaTime) override;

        void fixedUpdate (float deltaTime) override;

    public:
        BreakoutApp ();

        void addPoints (int points);
    };
}