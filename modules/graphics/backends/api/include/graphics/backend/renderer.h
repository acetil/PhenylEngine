#pragma once

#include <vector>
#include <optional>
#include <memory>

#include "core/iresource.h"

#include "abstract_render_layer.h"
#include "buffer.h"
#include "pipeline.h"
#include "uniform_buffer.h"
#include "graphics/viewport.h"
#include "shader.h"
#include "texture.h"

#include "util/optional.h"
#include "graphics/image.h"
#include "graphics/graphics_properties.h"

namespace phenyl::graphics {
    // TODO
    namespace detail {
        extern phenyl::Logger RENDERER_LOGGER;
    }

    class Renderer : public core::IResource {
    private:
        std::vector<std::unique_ptr<AbstractRenderLayer>> layers;
    protected:
        virtual std::unique_ptr<IBuffer> makeRendererBuffer (std::size_t startCapacity, std::size_t elementSize, bool isIndex) = 0;
        virtual std::unique_ptr<IUniformBuffer> makeRendererUniformBuffer (bool readable) = 0;
        virtual std::unique_ptr<IImageTexture> makeRendererImageTexture (const TextureProperties& properties) = 0;
        virtual std::unique_ptr<IImageArrayTexture> makeRendererArrayTexture (const TextureProperties& properties, std::uint32_t width, std::uint32_t height) = 0;
        virtual std::unique_ptr<IFrameBuffer> makeRendererFrameBuffer (const FrameBufferProperties& properties, std::uint32_t width, std::uint32_t height) = 0;

        void layerRender () {
            for (auto& i : layers) {
                i->render();
            }
        }
    public:
        virtual ~Renderer() = default;

        virtual double getCurrentTime () = 0;

        virtual void clearWindow () = 0;

        virtual void finishRender () = 0;

        virtual PipelineBuilder buildPipeline () = 0;

        virtual void loadDefaultShaders () = 0;

        virtual Viewport& getViewport () = 0;
        virtual const Viewport& getViewport () const = 0;

        template <typename T>
        Buffer<T> makeBuffer (std::size_t capacity, bool isIndex = false) {
            return Buffer<T>(makeRendererBuffer(sizeof(T) * capacity, sizeof(T), isIndex));
        }

        RawBuffer makeRawBuffer (std::size_t stride, std::size_t capacity, bool isIndex = false) {
            return RawBuffer{makeRendererBuffer(capacity * stride, stride, isIndex)};
        }

        template <typename T, typename ...Args>
        UniformBuffer<T> makeUniformBuffer (bool readable, Args&&...args) {
            return UniformBuffer<T>(makeRendererUniformBuffer(readable), std::forward<Args>(args)...);
        }

        template <typename T, typename ...Args>
        UniformBuffer<T> makeUniformBuffer (Args&&...args) {
            return UniformBuffer<T>(makeRendererUniformBuffer(false), std::forward<Args>(args)...);
        }

        template <typename T>
        UniformArrayBuffer<T> makeUniformArrayBuffer (std::size_t capacity = 8) {
            return UniformArrayBuffer<T>(makeRendererUniformBuffer(false), capacity);
        }

        RawUniformBuffer makeRawUniformBuffer (std::size_t size, bool readable = false) {
            return RawUniformBuffer{makeRendererUniformBuffer(readable), size};
        }

        ImageTexture makeTexture (const TextureProperties& properties, const Image& image) {
            auto texture = makeImageTexture(properties);
            texture.upload(image);

            return texture;
        }

        ImageTexture makeImageTexture (const TextureProperties& properties) {
            return ImageTexture{makeRendererImageTexture(properties)};
        }

        ImageArrayTexture makeArrayTexture (const TextureProperties& properties, std::uint32_t width, std::uint32_t height) {
            return ImageArrayTexture{makeRendererArrayTexture(properties, width, height)};
        }

        FrameBuffer makeFrameBuffer (const FrameBufferProperties& properties, std::uint32_t width, std::uint32_t height) {
            return FrameBuffer{makeRendererFrameBuffer(properties, width, height)};
        }

        template <std::derived_from<AbstractRenderLayer> T, typename ...Args>
        T& addLayer (Args&&... args) requires std::constructible_from<T, Args&&...> {
            auto* ptr = layers.emplace_back(std::make_unique<T>(std::forward<Args>(args)...)).get();

            std::sort(layers.begin(), layers.end(), [] (const auto& lhs, const auto& rhs) {
                return lhs->getPriority() < rhs->getPriority();
            });

            ptr->init(*this);

            PHENYL_LOGI(detail::RENDERER_LOGGER, "Added render layer \"{}\"", ptr->getName());

            return static_cast<T&>(*ptr);
        }

        void clearLayers () {
            layers.clear();
        }

        virtual void render () = 0;
    };

    std::unique_ptr<Renderer> MakeGLRenderer (const GraphicsProperties& properties);
    std::unique_ptr<Renderer> MakeVulkanRenderer (const GraphicsProperties& properties);
}
