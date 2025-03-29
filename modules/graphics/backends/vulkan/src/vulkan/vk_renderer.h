#pragma once

#include <memory>

#include "vk_device.h"
#include "graphics/backend/renderer.h"
#include "glfw/glfw_viewport.h"

#include "vulkan_headers.h"

namespace phenyl::vulkan {
    class VulkanViewport;

    class VulkanRenderer : public graphics::Renderer {
    private:
        static std::vector<const char*> GatherValidationLayers ();
        static void FilterValidationLayers (std::vector<const char*>& layers);

        static std::vector<const char*> GatherExtensions (const graphics::GraphicsProperties& properties, const VulkanViewport& viewport);
        static void CheckExtensions (const std::vector<const char*>& extensions);

        std::unique_ptr<VulkanViewport> viewport;
        VkInstance instance{};
        VkSurfaceKHR surface{};
        std::unique_ptr<VulkanDevice> device;
        std::unique_ptr<VulkanSwapChain> swapChain;

        VkDebugUtilsMessengerEXT debugMessenger{};

        VkInstance createVkInstance (const graphics::GraphicsProperties& properties);

        VkDebugUtilsMessengerCreateInfoEXT getDebugMessengerCreateInfo ();
        void setupDebugMessenger ();
        void destroyDebugMessenger ();
    protected:
        std::unique_ptr<graphics::IBuffer> makeRendererBuffer (std::size_t startCapacity, std::size_t elementSize) override;
        std::unique_ptr<graphics::IUniformBuffer> makeRendererUniformBuffer (bool readable) override;
        std::unique_ptr<graphics::IImageTexture> makeRendererImageTexture (const graphics::TextureProperties& properties) override;
        std::unique_ptr<graphics::IImageArrayTexture> makeRendererArrayTexture (const graphics::TextureProperties& properties, std::uint32_t width, std::uint32_t height) override;
        std::unique_ptr<graphics::IFrameBuffer> makeRendererFrameBuffer (const graphics::FrameBufferProperties& properties, std::uint32_t width, std::uint32_t height) override;

    public:
        VulkanRenderer (const graphics::GraphicsProperties& properties, std::unique_ptr<VulkanViewport> vkViewport);
        ~VulkanRenderer () override;

        double getCurrentTime () override;
        void clearWindow () override;
        void render() override;
        void finishRender () override;
        graphics::PipelineBuilder buildPipeline () override;
        void loadDefaultShaders () override;
        graphics::Viewport& getViewport () override;
        const graphics::Viewport& getViewport() const override;
        std::string_view getName() const noexcept override;
    };
}