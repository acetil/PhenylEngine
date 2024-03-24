#pragma once

#include <vector>

#include "graphics/maths_headers.h"

#include "common/input/input_source.h"
#include "common/input/proxy_source.h"

namespace phenyl::graphics {
    class Viewport {
    public:
        virtual ~Viewport() = default;

        [[nodiscard]] virtual bool shouldClose () const = 0;
        virtual void poll () = 0;
        [[nodiscard]] virtual glm::ivec2 getResolution () const = 0;
        [[nodiscard]] virtual glm::vec2 getCursorPos () const = 0;

        virtual std::vector<std::shared_ptr<phenyl::common::InputSource>> getInputSources () const = 0;
        virtual std::vector<std::shared_ptr<phenyl::common::ProxySource>> getProxySources () const = 0;
    };
}