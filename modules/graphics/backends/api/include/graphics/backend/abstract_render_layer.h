#pragma once

#include <string_view>

namespace phenyl::graphics {
    class Renderer;

    class AbstractRenderLayer {
    public:
        explicit AbstractRenderLayer (int priority) : m_priority{priority} {}
        virtual ~AbstractRenderLayer () = default;

        [[nodiscard]] virtual std::string_view getName () const = 0;
        [[nodiscard]] int priority () const {
            return m_priority;
        }

        virtual void init (Renderer& renderer) = 0;
        virtual void render () = 0;

    private:
        int m_priority;
    };
}