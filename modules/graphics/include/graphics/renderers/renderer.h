#pragma once

#include <vector>
#include <optional>
#include <memory>

#include "graphics/shaders/shaders.h"
#include "graphics/pipeline/pipeline_stage.h"
#include "common/input/input_source.h"
#include "runtime/iresource.h"

#include "util/optional.h"
#include "common/input/proxy_source.h"
#include "graphics/viewport.h"

namespace phenyl::graphics {
//#ifndef WINDOW_CALLBACKS_H
    class WindowCallbackContext;
//#endif
    class GraphicsTexture;
    class FrameBuffer {
    public:
        virtual void bind () = 0;
    };

    class Renderer : public runtime::IResource {
    protected:
        virtual std::shared_ptr<RendererBufferHandle> makeBufferHandle () = 0;
    public:
        virtual ~Renderer() = default;

        virtual double getCurrentTime () = 0;

        virtual void clearWindow () = 0;

        virtual FrameBuffer* getWindowBuffer () = 0;

        virtual void finishRender () = 0;

        virtual GraphicsTexture loadTexture (int width, int height, unsigned char* data) = 0;
        virtual GraphicsTexture loadTextureGrey (int width, int height, unsigned char* data) = 0;
        virtual void reloadTexture (unsigned int textureId, int width, int height, unsigned char* data) = 0;

        virtual void bindTexture (unsigned int textureId) = 0;
        virtual void destroyTexture (unsigned int textureId) = 0;

        virtual PipelineStage buildPipelineStage (PipelineStageBuilder& stageBuilder) = 0;

        virtual void loadDefaultShaders () = 0;

        virtual Viewport& getViewport () = 0;
        virtual const Viewport& getViewport () const = 0;

        template <typename T>
        Buffer<T> makeBuffer (unsigned int bufferCapacity, std::size_t elementSize=1) {
            return Buffer<T>{makeBufferHandle(), elementSize, bufferCapacity};
        }
    };
    class GraphicsTexture {
        Renderer* renderer;
        unsigned int textureId;
    public:
        GraphicsTexture () : renderer(nullptr), textureId (0) {};
        GraphicsTexture (Renderer* render, unsigned int id) : renderer(render), textureId(id) {};

        GraphicsTexture (const GraphicsTexture&) = delete;
        GraphicsTexture (GraphicsTexture&& other) noexcept : renderer{other.renderer}, textureId{other.textureId} {
            other.renderer = nullptr;
            other.textureId = 0;
        }

        GraphicsTexture& operator= (const GraphicsTexture&) = delete;
        GraphicsTexture& operator= (GraphicsTexture&& other) noexcept {
            if (renderer && textureId) {
                renderer->destroyTexture(textureId);
            }

            renderer = other.renderer;
            textureId = other.textureId;
            other.renderer = nullptr;
            other.textureId = 0;
            return *this;
        }

        ~GraphicsTexture () {
            if (renderer && textureId) {
                renderer->destroyTexture(textureId);
            }
        }
        void bindTexture () {
            renderer->bindTexture(textureId);
        }
        void reload (int width, int height, unsigned char* data) {
            renderer->reloadTexture(textureId, width, height, data);
        }
    };

}
