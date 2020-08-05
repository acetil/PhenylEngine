#include "collision_component.h"
#include <vector>
#ifndef COLLISIONS_H
#define COLLISIONS_H
namespace physics {
    void checkCollisionsEntity (CollisionComponent* comp, int numEntities, [[maybe_unused]] int direction, std::vector<std::tuple<int, int, glm::vec2>>* collisionResults);
}
#endif //COLLISIONS_H
