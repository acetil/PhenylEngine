#pragma once

#include "vulkan_headers.h"

#include "graphics/backend/framebuffer.h"
#include "init/vk_swap_chain.h"
#include "memory/vk_transfer_manager.h"
#include "texture/vk_image.h"

namespace phenyl::vulkan {
    class VulkanCommandBuffer2;

    class IVulkanFrameBuffer {
    public:
        virtual ~IVulkanFrameBuffer () = default;

        virtual void prepareRendering (VulkanCommandBuffer2& cmd) = 0;
        virtual const VkRenderingInfo* getRenderingInfo () const noexcept = 0;

        virtual VkViewport viewport () const noexcept = 0;
        virtual VkRect2D scissor () const noexcept = 0;
    };

    class VulkanFrameBuffer : public graphics::IFrameBuffer, public IVulkanFrameBuffer {
    private:
        class Sampler : public graphics::ISampler {
        public:
            std::size_t hash () const noexcept override {
                return 1;
            }
        };

        Sampler dummy{};
        std::optional<VkClearColorValue> clearColor;

        VkRenderingInfo renderingInfo{};
    public:
        void prepareRendering (VulkanCommandBuffer2& cmd) override;
        const VkRenderingInfo* getRenderingInfo () const noexcept override;
        VkViewport viewport () const noexcept override;
        VkRect2D scissor () const noexcept override;

        void clear (glm::vec4 clearColor) override;

        graphics::ISampler* getSampler () noexcept override;

        graphics::ISampler* getDepthSampler() noexcept override;

        glm::ivec2 getDimensions() const noexcept override;
    };

    class VulkanWindowFrameBuffer : public IVulkanFrameBuffer {
    private:
        VulkanResources& resources;
        TransferManager& transferManager;

        VulkanSwapChain* swapChain;

        SwapChainImage swapChainImage;
        VkImageLayout currLayout;

        VulkanImage depthImage{};
        VulkanImageView depthImageView{};

        bool isFirstRender = true;

        VkRenderingAttachmentInfo colorAttachment;
        VkRenderingAttachmentInfo depthAttachment;
        VkRenderingInfo renderingInfo;
    public:
        VulkanWindowFrameBuffer (VulkanResources& resources, TransferManager& transferManager);

        VkViewport viewport () const noexcept override;
        VkRect2D scissor () const noexcept override;

        bool acquireFrame (const VulkanSemaphore& waitSem);
        void onSwapChainRecreate (VulkanSwapChain* newSwapChain);
        void doPresentTransition (VulkanCommandBuffer2& cmd);

        void setClearColor (glm::vec4 color);

        void prepareRendering (VulkanCommandBuffer2& cmd) override;
        const VkRenderingInfo* getRenderingInfo () const noexcept override;
    };
}
