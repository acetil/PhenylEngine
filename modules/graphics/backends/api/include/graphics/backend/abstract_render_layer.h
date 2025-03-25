#pragma once

#include <string_view>

namespace phenyl::graphics {
    class Renderer;

    class AbstractRenderLayer {
    private:
        int priority;
    public:
        explicit AbstractRenderLayer (int priority) : priority{priority} {}
        virtual ~AbstractRenderLayer () = default;

        [[nodiscard]] virtual std::string_view getName () const = 0;
        [[nodiscard]] int getPriority () const {
            return priority;
        }

        virtual void init (Renderer& renderer) = 0;
        virtual void render () = 0;
    };
}