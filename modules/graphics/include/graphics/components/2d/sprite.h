#pragma once

#include <utility>

#include "graphics/maths_headers.h"
#include "common/assets/asset.h"
#include "common/serializer_intrusive.h"

namespace phenyl::graphics {
    class SpriteTexture;

    class Sprite2D {
    private:
        common::Asset<SpriteTexture> texture;
        bool updated = false;
        glm::vec2 topLeft;
        glm::vec2 bottomRight;
        void update () {
            topLeft = texture->topLeft();
            bottomRight = texture->bottomRight();
            updated = true;
        }

        PHENYL_SERIALIZE_INTRUSIVE(Sprite2D);
        friend class EntityRenderLayer;
    public:
        explicit operator bool () const {
            return (bool)texture;
        }

        Sprite2D& operator= (common::Asset<SpriteTexture> newTexture) {
            texture = std::move(newTexture);
            updated = true;
            return *this;
        }

        inline glm::vec2 getTopLeft () const {
            return topLeft;
        }

        inline glm::vec2 getBottomRight () const {
            return bottomRight;
        }
    };
}