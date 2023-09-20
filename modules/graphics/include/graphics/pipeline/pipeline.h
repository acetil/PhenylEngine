#pragma once

#include <memory>
#include <utility>

//#include "graphics/shaders/shader_new.h"
//#include "graphics/renderers/buffer_new.h"
#include "graphics/renderers/renderer.h"

namespace phenyl::graphics {

    /*c

    template <typename T>
    class BufferNew {
    private:

    };*/

    template <typename ...Args>
    class Pipeline {
    private:
    public:
        virtual ~Pipeline() = default;

        virtual void init (Renderer* renderer) = 0;

        virtual void bufferData (Args&... args) = 0;
        virtual void render () = 0;
    };
}