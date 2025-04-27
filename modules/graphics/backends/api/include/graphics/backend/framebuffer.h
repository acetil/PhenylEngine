#pragma once

#include <memory>

#include "texture.h"
#include "logging/logging.h"

namespace phenyl::graphics {
    // enum class DepthStencilFormat {
    //
    // };

    struct FrameBufferProperties {
        std::optional<ImageFormat> format = std::nullopt;
        std::optional<ImageFormat> depthFormat = std::nullopt;

        // TODO: move to per pipeline
        TextureWrapping wrapping = TextureWrapping::REPEAT;
        TextureBorderColor borderColor = TextureBorderColor::TRANSPARENT;
        float depthBorder = 1.0f;
    };

    class IFrameBuffer {
    public:
        virtual ~IFrameBuffer () = default;

        virtual void clear (glm::vec4 clearColor) = 0;
        virtual ISampler* getSampler () noexcept = 0;
        virtual ISampler* getDepthSampler () noexcept = 0;
        virtual glm::ivec2 getDimensions () const noexcept = 0;
    };

    class FrameBuffer {
    private:
        std::unique_ptr<IFrameBuffer> rendererFB;
    public:
        FrameBuffer () : rendererFB{nullptr} {}
        explicit FrameBuffer (std::unique_ptr<IFrameBuffer> fb) : rendererFB{std::move(fb)} {}

        IFrameBuffer& getUnderlying () noexcept {
            PHENYL_DASSERT(rendererFB);
            return *rendererFB;
        }

        const IFrameBuffer& getUnderlying () const noexcept {
            PHENYL_DASSERT(rendererFB);
            return *rendererFB;
        }

        ISampler& sampler () const noexcept {
            PHENYL_DASSERT_MSG(rendererFB->getSampler(), "Attempted to get sampler from framebuffer without color attachment!");
            return *rendererFB->getSampler();
        }

        ISampler& depthSampler () const noexcept {
            PHENYL_DASSERT_MSG(rendererFB->getDepthSampler(), "Attempted to get depth sampler from framebuffer without depth attachment!");
            return *rendererFB->getDepthSampler();
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
