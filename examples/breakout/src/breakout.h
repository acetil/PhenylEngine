#pragma once

#include <phenyl/application.h>

namespace breakout {
    class BreakoutApp : public phenyl::Application {
    private:
        void init () override;

        void update (double deltaTime) override;

        void fixedUpdate (float deltaTime) override;

    public:
        BreakoutApp ();
    };
}