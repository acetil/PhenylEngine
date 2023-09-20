#pragma once

#include "graphics/graphics_headers.h"

#include "graphics/renderers/buffer.h"
#include "graphics/renderers/renderer_buffer.h"

namespace phenyl::graphics {
    //class GLPipelineStage;

    class GlBuffer : public RendererBufferHandle {
    private:
        GLuint bufferId{};
        std::unique_ptr<unsigned char[]> data;
        std::size_t currentSize{};
        std::size_t elementSize{};
        std::size_t maxSize{};
    public:
        explicit GlBuffer ();
        ~GlBuffer() override;

        void allocBufferMemory(std::size_t memSize) override;
        void setElementSize(std::size_t elementSize) override;
        void bufferItems(const void* _data, std::size_t size) override;
        void bufferData() override;
        void clearBuffer() override;

        void bindBuffer () const;

        [[nodiscard]] std::size_t getNumElements () const;

        //[[nodiscard]] GLuint getBufferId () const;
    };
}