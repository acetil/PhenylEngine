#pragma once

#include <vector>
#include <optional>
#include <memory>

#include "graphics/shaders/shaders.h"
#include "graphics/shaders/shader_new.h"
#include "graphics/pipeline/pipeline_stage.h"

#include "util/optional.h"

namespace graphics {
//#ifndef WINDOW_CALLBACKS_H
    class WindowCallbackContext;
//#endif
    class GraphicsTexture;
    class FrameBuffer {
    public:
        virtual void bind () = 0;
    };

    class Renderer {
    protected:
        virtual std::shared_ptr<RendererBufferHandle> makeBufferHandle () = 0;
    public:

        virtual ~Renderer() = default;

        virtual double getCurrentTime () = 0;

        virtual bool shouldClose () = 0;

        virtual void pollEvents () = 0;

        virtual void clearWindow () = 0;

        virtual FrameBuffer* getWindowBuffer () = 0;
        //virtual std::optional<ShaderProgram*> getProgram (std::string program) = 0;
        virtual util::Optional<ShaderProgramNew> getProgramNew (const std::string& program) = 0;

        virtual void finishRender () = 0;

        virtual GraphicsTexture loadTexture (int width, int height, unsigned char* data) = 0;
        virtual GraphicsTexture loadTextureGrey (int width, int height, unsigned char* data) = 0;

        virtual void bindTexture (unsigned int textureId) = 0;

        virtual void setupWindowCallbacks (std::unique_ptr<WindowCallbackContext> ctx) = 0;

        virtual void invalidateWindowCallbacks () = 0;

        virtual void addShader (const std::string& shaderName, const ShaderProgramBuilder& shaderBuilder) = 0;

        virtual PipelineStage buildPipelineStage (const PipelineStageBuilder& stageBuilder) = 0;

        template <typename T>
        Buffer<T> makeBuffer (unsigned int bufferCapacity, std::size_t elementSize=1) {
            return Buffer<T>{makeBufferHandle(), elementSize, bufferCapacity};
        }
    };
    class GraphicsTexture {
        Renderer* renderer;
        unsigned int textureId;
    public:
        GraphicsTexture () : renderer(nullptr), textureId (-1) {};
        GraphicsTexture (Renderer* render, unsigned int id) : renderer(render), textureId(id) {};
        void bindTexture () {
            renderer->bindTexture(textureId);
        }
    };

}
