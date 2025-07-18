#pragma once

#include <phenyl/application.h>
#include <phenyl/asset.h>
#include <phenyl/prefab.h>
#include <phenyl/ui/label.h>

namespace breakout {
class BreakoutApp : public phenyl::Application2D {
public:
    BreakoutApp (phenyl::ApplicationProperties properties);

    void addPoints (int points);
    void subtractLife ();

    void onWin ();

private:
    int m_totalPoints = 0;
    int m_lives;
    // phenyl::ui::Label pointsLabel;
    // phenyl::ui::Label livesLabel;
    phenyl::ui::LabelWidget* m_pointsLabel = nullptr;
    phenyl::ui::LabelWidget* m_livesLabel = nullptr;
    std::shared_ptr<phenyl::Prefab> m_tilePrefab;

    void init () override;
    void postInit () override;
};
} // namespace breakout
