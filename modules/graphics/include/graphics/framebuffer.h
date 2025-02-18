#pragma once

#include <memory>

#include "texture.h"
#include "logging/logging.h"

namespace phenyl::graphics {
    // enum class DepthStencilFormat {
    //
    // };

    struct FrameBufferProperties {
        ImageFormat format = ImageFormat::RGBA;
    };

    class IFrameBuffer {
    public:
        virtual ~IFrameBuffer () = default;

        virtual void clear (glm::vec4 clearColor) = 0;
        virtual const ISampler& getSampler () const noexcept = 0;
        virtual glm::ivec2 getDimensions () const noexcept = 0;
    };

    class FrameBufferTexture : public Texture {
    private:
        IFrameBuffer* fb;

    public:
        FrameBufferTexture (IFrameBuffer* fb) : Texture{fb ? fb->getSampler().hash() : 0}, fb{fb} {}

        const ISampler& sampler () const noexcept override {
            PHENYL_ASSERT(fb);
            return fb->getSampler();
        }
    };

    class FrameBuffer {
    private:
        std::unique_ptr<IFrameBuffer> rendererFB;
        FrameBufferTexture fbTexture;
    public:
        FrameBuffer () : rendererFB{nullptr}, fbTexture{nullptr} {}
        explicit FrameBuffer (std::unique_ptr<IFrameBuffer> fb) : rendererFB{std::move(fb)}, fbTexture{rendererFB.get()} {}

        IFrameBuffer& getUnderlying () noexcept {
            PHENYL_DASSERT(rendererFB);
            return *rendererFB;
        }

        const IFrameBuffer& getUnderlying () const noexcept {
            PHENYL_DASSERT(rendererFB);
            return *rendererFB;
        }

        FrameBufferTexture& texture () noexcept {
            return fbTexture;
        }

        const FrameBufferTexture& texture () const noexcept {
            return fbTexture;
        }

        glm::ivec2 dimensions () const noexcept {
            PHENYL_DASSERT(rendererFB);
            return rendererFB->getDimensions();
        }

        std::size_t width () const noexcept {
            return dimensions().x;
        }

        std::size_t height () const noexcept {
            return dimensions().y;
        }

        void clear (glm::vec4 clearColor = {0.0f, 0.0f, 0.0f, 1.0f}) {
            PHENYL_DASSERT(rendererFB);
            rendererFB->clear(clearColor);
        }
    };
}
