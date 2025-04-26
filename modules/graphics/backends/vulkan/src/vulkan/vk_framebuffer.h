#pragma once

#include <unordered_set>

#include "util/hash.h"

#include "vulkan_headers.h"

#include "graphics/backend/framebuffer.h"
#include "init/vk_swap_chain.h"
#include "memory/vk_transfer_manager.h"
#include "texture/vk_image.h"

namespace phenyl::vulkan {
    struct FrameBufferLayout {
        std::vector<VkFormat> colorFormats{};
        VkFormat depthFormat = VK_FORMAT_UNDEFINED;
        VkFormat stencilFormat = VK_FORMAT_UNDEFINED;

        bool operator== (const FrameBufferLayout&) const = default;

        VkPipelineRenderingCreateInfo getInfo () const noexcept;
    };
}


template<>
struct std::hash<phenyl::vulkan::FrameBufferLayout> {
    std::size_t operator() (const phenyl::vulkan::FrameBufferLayout& layout) const noexcept {
        return phenyl::util::HashAll(layout.colorFormats, layout.depthFormat, layout.stencilFormat);
    }
};

namespace phenyl::vulkan {
    class VulkanCommandBuffer2;

    class IVulkanFrameBuffer {
    private:
        const FrameBufferLayout* fbLayout;
    public:
        IVulkanFrameBuffer (const FrameBufferLayout* fbLayout) : fbLayout{fbLayout} {
            PHENYL_DASSERT(fbLayout);
        }
        virtual ~IVulkanFrameBuffer () = default;

        const FrameBufferLayout* layout () const noexcept {
            return fbLayout;
        }

        virtual void prepareRendering (VulkanCommandBuffer2& cmd) = 0;
        virtual const VkRenderingInfo* getRenderingInfo () const noexcept = 0;

        virtual VkViewport viewport () const noexcept = 0;
        virtual VkRect2D scissor () const noexcept = 0;
    };

    class FrameBufferLayoutManager {
    private:
        std::unordered_set<FrameBufferLayout> layoutCache;

    public:
        FrameBufferLayoutManager ();

        const FrameBufferLayout* cacheLayout (FrameBufferLayout&& layout);
    };

    class VulkanFrameBuffer : public graphics::IFrameBuffer, public IVulkanFrameBuffer {
    private:
        struct FrameBufferSampler {
        };

        std::vector<CombinedSampler> colorSamplers;
        std::vector<VkRenderingAttachmentInfo> colorAttachments;

        std::optional<CombinedSampler> depthSampler;
        VkRenderingAttachmentInfo depthAttachment;

        VkExtent2D extent;
        std::optional<VkClearColorValue> clearColor;

        VkRenderingInfo renderingInfo{};
    public:
        VulkanFrameBuffer (VulkanResources& resources, FrameBufferLayoutManager& layoutManager, const graphics::FrameBufferProperties& properties, std::uint32_t width, std::uint32_t height);

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
        VulkanWindowFrameBuffer (VulkanResources& resources, TransferManager& transferManager, FrameBufferLayoutManager& layoutManager, VkFormat colorFormat);

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
