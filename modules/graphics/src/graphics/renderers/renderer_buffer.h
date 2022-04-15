#pragma once

#include "graphics/renderers/buffer.h"

namespace graphics {
    class RendererBufferHandle {
    public:
        virtual ~RendererBufferHandle() = default;

        virtual void allocBufferMemory (std::size_t memSize) = 0;
        virtual void setElementSize (std::size_t elementSize) = 0;
        virtual void bufferData () = 0;
        virtual void bufferItems (const void* data, std::size_t size) = 0;
        virtual void clearBuffer () = 0;
    };
}
