#pragma once

#include "common/serializer.h"

#include "bullet.h"
#include "player.h"

namespace game {
    PHENYL_SERIALIZE(Bullet, {})
    PHENYL_SERIALIZE(Player, {})
}