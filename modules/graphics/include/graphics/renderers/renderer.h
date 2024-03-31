#pragma once

#include <vector>
#include <optional>
#include <memory>

#include "graphics/shaders/shaders.h"
#include "common/input/input_source.h"
#include "runtime/iresource.h"
#include "buffer.h"
#include "uniform_buffer.h"

#include "util/optional.h"
#include "common/input/proxy_source.h"
#include "graphics/viewport.h"
#include "pipeline.h"
#include "graphics/abstract_render_layer.h"
#include "graphics/detail/loggers.h"
#include "graphics/image.h"
#include "graphics/renderers/texture.h"

namespace phenyl::graphics {
    class Renderer : public runtime::IResource {
    private:
        std::vector<std::unique_ptr<AbstractRenderLayer>> layers;
    protected:
        virtual std::unique_ptr<IBuffer> makeRendererBuffer (std::size_t startCapacity, std::size_t elementSize) = 0;
        virtual std::unique_ptr<IUniformBuffer> makeRendererUniformBuffer (bool readable) = 0;
        virtual std::unique_ptr<IImageTexture> makeRendererImageTexture (const TextureProperties& properties) = 0;
        virtual std::unique_ptr<IImageArrayTexture> makeRendererArrayTexture (const TextureProperties& properties, std::uint32_t width, std::uint32_t height) = 0;

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
        Buffer<T> makeBuffer (std::size_t capacity) {
            return Buffer<T>(makeRendererBuffer(sizeof(T) * capacity, sizeof(T)));
        }

        template <typename T, typename ...Args>
        UniformBuffer<T> makeUniformBuffer (bool readable, Args&&...args) {
            return UniformBuffer<T>(makeRendererUniformBuffer(readable), std::forward<Args>(args)...);
        }

        template <typename T, typename ...Args>
        UniformBuffer<T> makeUniformBuffer (Args&&...args) {
            return UniformBuffer<T>(makeRendererUniformBuffer(false), std::forward<Args>(args)...);
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
}
