#pragma once

#include <phenyl/application.h>
#include <phenyl/asset.h>
#include <phenyl/ui/label.h>
#include <phenyl/prefab.h>

namespace breakout {
    class BreakoutApp : public phenyl::Application2D {
    private:
        int totalPoints = 0;
        int lives;
        // phenyl::ui::Label pointsLabel;
        // phenyl::ui::Label livesLabel;
        phenyl::ui::LabelWidget* pointsLabel = nullptr;
        phenyl::ui::LabelWidget* livesLabel = nullptr;
        phenyl::Asset<phenyl::Prefab> tilePrefab;

        void init () override;
    public:
        BreakoutApp (phenyl::ApplicationProperties properties);

        void addPoints (int points);
        void subtractLife ();

        void onWin ();
    };
}