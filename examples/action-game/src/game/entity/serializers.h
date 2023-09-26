#pragma once

#include <phenyl/serialization.h>

#include "bullet.h"
#include "player.h"

namespace test {
    PHENYL_SERIALIZE(Bullet, {})
    PHENYL_SERIALIZE(Player, {
        PHENYL_MEMBER_NAMED(bulletPrefab, "bullet_prefab");
    })
}