#pragma once

#include "core/iresource.h"
#include "graphics/maths_headers.h"

#include <vector>

namespace phenyl::core {
class GameInput;
}

namespace phenyl::graphics {
class IViewportUpdateHandler {
public:
    virtual ~IViewportUpdateHandler () = default;

    virtual void onViewportResize (glm::ivec2 oldResolution, glm::ivec2 newResolution) = 0;
};

class Viewport : public core::IResource {
public:
    ~Viewport () override = default;

    [[nodiscard]] virtual bool shouldClose () const = 0;
    virtual void poll () = 0;
    [[nodiscard]] virtual glm::ivec2 getResolution () const = 0;
    [[nodiscard]] virtual glm::vec2 getContentScale () const = 0;

    virtual void addInputDevices (core::GameInput& manager) = 0;

    virtual void addUpdateHandler (IViewportUpdateHandler* handler) = 0;
};
} // namespace phenyl::graphics
