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
    public:
        IVulkanFrameBuffer (const FrameBufferLayout* fbLayout) : m_layout{fbLayout} {
            PHENYL_DASSERT(fbLayout);
        }
        virtual ~IVulkanFrameBuffer () = default;

        const FrameBufferLayout* layout () const noexcept {
            return m_layout;
        }

        virtual void prepareRendering (VulkanCommandBuffer2& cmd) = 0;
        virtual const VkRenderingInfo* getRenderingInfo () const noexcept = 0;

        virtual VkViewport viewport () const noexcept = 0;
        virtual VkRect2D scissor () const noexcept = 0;

    private:
        const FrameBufferLayout* m_layout;
    };

    class FrameBufferLayoutManager {
    public:
        FrameBufferLayoutManager ();

        const FrameBufferLayout* cacheLayout (FrameBufferLayout&& layout);

    private:
        std::unordered_set<FrameBufferLayout> m_layoutCache;
    };

    class VulkanFrameBuffer : public graphics::IFrameBuffer, public IVulkanFrameBuffer {
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

    private:
        struct FrameBufferSampler {
        };

        std::vector<CombinedSampler> m_colorSamplers;
        std::vector<VkRenderingAttachmentInfo> m_colorAttachments;

        std::optional<CombinedSampler> m_depthSampler;
        VkRenderingAttachmentInfo m_depthAttachment;

        VkExtent2D m_extent;
        std::optional<VkClearColorValue> m_clearColor;

        VkRenderingInfo m_renderingInfo{};
    };

    class VulkanWindowFrameBuffer : public IVulkanFrameBuffer {
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

    private:
        VulkanResources& m_resources;
        TransferManager& m_transferManager;

        VulkanSwapChain* m_swapChain;

        SwapChainImage m_swapChainImage;
        VkImageLayout m_currLayout;

        VulkanImage m_depthImage{};
        VulkanImageView m_depthImageView{};

        bool m_isFirstRender = true;

        VkRenderingAttachmentInfo m_colorAttachment;
        VkRenderingAttachmentInfo m_depthAttachment;
        VkRenderingInfo m_renderingInfo;
    };
}
