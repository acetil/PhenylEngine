#pragma once

#include <string>

#include "graphics_headers.h"
#include "graphics/maths_headers.h"
#include "runtime/iresource.h"

namespace phenyl::graphics {
    class Camera : public runtime::IResource {
    private:
        glm::mat4 camMatrix{};
        glm::mat4 positionMat{};
        glm::mat4 scaleMat{};
    public:
        [[maybe_unused]] void translate (float x, float y);

        [[maybe_unused]] void scale (float scale);

        [[maybe_unused]] void setPosition (float x, float y);
        void setPos2D (glm::vec2 newPos);

        [[maybe_unused]] void setScale (float scale);
        [[nodiscard]] glm::mat4 getCamMatrix () const ;
        static std::string getUniformName () ;
        Camera();

        glm::vec2 getWorldPos2D (glm::vec2 screenPos);

        std::string_view getName() const noexcept override;
    };
}
