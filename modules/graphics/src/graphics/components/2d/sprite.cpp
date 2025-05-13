#include "graphics/components/2d/sprite.h"

#include "core/assets/assets.h"
#include "core/serialization/serializer_impl.h"

using namespace phenyl::graphics;

namespace phenyl::graphics {
PHENYL_SERIALIZABLE(Sprite2D, PHENYL_SERIALIZABLE_MEMBER(texture))
}
