#pragma once

#include <string>

#include "viewport.h"
#include "graphics/maths_headers.h"
#include "core/iresource.h"

namespace phenyl::graphics {
    class Camera : public core::IResource, public IViewportUpdateHandler {
    private:
        glm::mat4 camMatrix{};
        glm::mat4 positionMat{};
        glm::mat4 scaleMat{};
        glm::vec2 resolution;
    public:
        [[maybe_unused]] void translate (float x, float y);

        [[maybe_unused]] void scale (float scale);

        [[maybe_unused]] void setPosition (float x, float y);
        void setPos2D (glm::vec2 newPos);

        [[maybe_unused]] void setScale (float scale);
        [[nodiscard]] glm::mat4 getCamMatrix () const ;
        static std::string getUniformName () ;
        explicit Camera (glm::vec2 resolution);

        glm::vec2 getWorldPos2D (glm::vec2 screenPos) const;

        std::string_view getName() const noexcept override;

        void onViewportResize (glm::ivec2 oldResolution, glm::ivec2 newResolution) override;
    };
}
