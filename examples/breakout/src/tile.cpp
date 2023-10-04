#include <iostream>
#include <phenyl/signals/physics.h>

#include "tile.h"

using namespace breakout;

void breakout::initTile (breakout::BreakoutApp* app, phenyl::ComponentManager& componentManager) {
    app->addComponent<Tile>();

    componentManager.handleSignal<phenyl::signals::OnCollision, Tile>([app] (const phenyl::signals::OnCollision& signal, phenyl::Entity entity, Tile& tile) {
        if (!--tile.health) {
            app->addPoints(tile.points);
            entity.remove();
            std::cout << "Tile destroyed!\n";
        }
    });
}