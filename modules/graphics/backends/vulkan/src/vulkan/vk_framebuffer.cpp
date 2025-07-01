#include "vk_framebuffer.h"

using namespace phenyl::vulkan;

static FrameBufferLayout GetFrameBufferLayout (const phenyl::graphics::FrameBufferProperties& properties) {
    FrameBufferLayout layout{};

    if (properties.format) {
        layout.colorFormats.emplace_back(FormatToVulkan(*properties.format));
    }

    if (properties.depthFormat) {
        layout.depthFormat = FormatToVulkan(*properties.depthFormat);
    }

    return layout;
}

VkPipelineRenderingCreateInfo FrameBufferLayout::getInfo () const noexcept {
    return VkPipelineRenderingCreateInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
      .colorAttachmentCount = static_cast<std::uint32_t>(colorFormats.size()),
      .pColorAttachmentFormats = colorFormats.data(),
      .depthAttachmentFormat = depthFormat,
      .stencilAttachmentFormat = stencilFormat};
}

FrameBufferLayoutManager::FrameBufferLayoutManager () : m_layoutCache{} {}

const FrameBufferLayout* FrameBufferLayoutManager::cacheLayout (FrameBufferLayout&& layout) {
    auto it = m_layoutCache.find(layout);
    if (it != m_layoutCache.end()) {
        return &*it;
    }

    return &*m_layoutCache.emplace(std::move(layout)).first;
}

VulkanFrameBuffer::VulkanFrameBuffer (VulkanResources& resources, FrameBufferLayoutManager& layoutManager,
    const graphics::FrameBufferProperties& properties, std::uint32_t width, std::uint32_t height) :
    IVulkanFrameBuffer{layoutManager.cacheLayout(GetFrameBufferLayout(properties))},
    m_extent{width, height} {
    const auto* fbLayout = layout();

    for (auto colorFormat : fbLayout->colorFormats) {
        m_colorSamplers.emplace_back(resources, properties);
        m_colorSamplers.back().recreate(resources,
            VulkanImage{resources, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT,
              VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, width, height});

        m_colorAttachments.emplace_back(VkRenderingAttachmentInfo{.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
          .imageView = m_colorSamplers.back().view(),
          .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
          .resolveMode = VK_RESOLVE_MODE_NONE,
          .storeOp = VK_ATTACHMENT_STORE_OP_STORE});
    }

    if (properties.depthFormat) {
        m_depthSampler =
            CombinedSampler{resources, properties, VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL};
        m_depthSampler->recreate(resources,
            VulkanImage{resources, fbLayout->depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
              VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, width, height},
            VK_IMAGE_ASPECT_DEPTH_BIT);

        m_depthAttachment = VkRenderingAttachmentInfo{
          .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
          .imageView = m_depthSampler->view(),
          .imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
          .resolveMode = VK_RESOLVE_MODE_NONE,
          .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        };
    }

    m_renderingInfo = VkRenderingInfo{
      .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
      .renderArea = VkRect2D{.offset = {0, 0}, .extent = m_extent},
      .layerCount = 1,
      .colorAttachmentCount = static_cast<std::uint32_t>(m_colorAttachments.size()),
      .pColorAttachments = m_colorAttachments.data(),
      .pDepthAttachment = m_depthSampler ? &m_depthAttachment : nullptr,
    };
}

void VulkanFrameBuffer::prepareRendering (VulkanCommandBuffer2& cmd) {
    for (std::size_t i = 0; i < m_colorSamplers.size(); i++) {
        m_colorSamplers[i].image().layoutTransition(cmd, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        if (m_clearColor) {
            m_colorAttachments[i].clearValue = VkClearValue{.color = *m_clearColor};
            m_colorAttachments[i].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        } else {
            m_colorAttachments[i].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        }
    }

    if (m_depthSampler) {
        m_depthSampler->image().layoutTransition(cmd, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

        if (m_clearColor) {
            m_depthAttachment.clearValue = VkClearValue{.depthStencil = VkClearDepthStencilValue{.depth = 1.0f}};
            m_depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        } else {
            m_depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        }
    }

    m_clearColor = std::nullopt;
}

const VkRenderingInfo* VulkanFrameBuffer::getRenderingInfo () const noexcept {
    return &m_renderingInfo;
}

VkViewport VulkanFrameBuffer::viewport () const noexcept {
    return VkViewport{
      .x = 0,
      .y = 0,
      .width = static_cast<float>(m_extent.width),
      .height = static_cast<float>(m_extent.height),
      .minDepth = 0.0f,
      .maxDepth = 1.0f,
    };
}

VkRect2D VulkanFrameBuffer::scissor () const noexcept {
    return VkRect2D{.offset = {0, 0}, .extent = m_extent};
}

void VulkanFrameBuffer::clear (glm::vec4 clearColor) {
    this->m_clearColor = VkClearColorValue{.float32 = {clearColor.r, clearColor.g, clearColor.b, clearColor.a}};
}

phenyl::graphics::ISampler* VulkanFrameBuffer::getSampler () noexcept {
    return !m_colorSamplers.empty() ? &m_colorSamplers[0] : nullptr;
}

phenyl::graphics::ISampler* VulkanFrameBuffer::getDepthSampler () noexcept {
    return m_depthSampler ? &*m_depthSampler : nullptr;
}

glm::ivec2 VulkanFrameBuffer::getDimensions () const noexcept {
    return {m_extent.width, m_extent.height};
}

VulkanWindowFrameBuffer::VulkanWindowFrameBuffer (VulkanResources& resources, TransferManager& transferManager,
    FrameBufferLayoutManager& layoutManager, VkFormat colorFormat) :
    IVulkanFrameBuffer{layoutManager.cacheLayout(FrameBufferLayout{
      .colorFormats = {colorFormat},
      .depthFormat = VK_FORMAT_D24_UNORM_S8_UINT,
    })},
    m_resources{resources},
    m_transferManager{transferManager} {
    m_colorAttachment = {.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
      .resolveMode = VK_RESOLVE_MODE_NONE,
      .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
      .clearValue = {
        .color = {0, 0, 0, 0},
      }};

    m_depthAttachment = {.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
      .resolveMode = VK_RESOLVE_MODE_NONE,
      .storeOp = VK_ATTACHMENT_STORE_OP_STORE, // TODO
      .clearValue = {
        .depthStencil =
            VkClearDepthStencilValue{
              .depth = 1.0f,
            },
      }};

    m_renderingInfo = {
      .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
      .layerCount = 1,
      .colorAttachmentCount = 1,
      .pColorAttachments = &m_colorAttachment,
      .pDepthAttachment = &m_depthAttachment,

      // TODO: stencil
    };
}

VkViewport VulkanWindowFrameBuffer::viewport () const noexcept {
    PHENYL_ASSERT(m_swapChain);

    auto extent = m_swapChain->extent();
    return VkViewport{
      .x = 0,
      .y = 0,
      .width = static_cast<float>(extent.width),
      .height = static_cast<float>(extent.height),
      .minDepth = 0.0f,
      .maxDepth = 1.0f,
    };
}

VkRect2D VulkanWindowFrameBuffer::scissor () const noexcept {
    return VkRect2D{.offset = {0, 0}, .extent = m_swapChain->extent()};
}

bool VulkanWindowFrameBuffer::acquireFrame (const VulkanSemaphore& waitSem) {
    PHENYL_ASSERT(m_swapChain);

    auto image = m_swapChain->acquireImage(waitSem);
    if (!image) {
        return false;
    }

    m_swapChainImage = *image;
    m_currLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    m_colorAttachment.imageView = m_swapChainImage.view;
    m_isFirstRender = true;

    return true;
}

void VulkanWindowFrameBuffer::onSwapChainRecreate (VulkanSwapChain* newSwapChain) {
    PHENYL_ASSERT(newSwapChain);

    m_swapChain = newSwapChain;

    m_renderingInfo.renderArea = {
      .offset = {0, 0},
      .extent = m_swapChain->extent(),
    };

    auto swapExtent = m_swapChain->extent();
    if (swapExtent.width == m_depthImage.width() && swapExtent.height == m_depthImage.height()) {
        // No need to change depth buffer
        return;
    }

    m_depthImage =
        VulkanImage{m_resources, VK_FORMAT_D24_UNORM_S8_UINT, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
          VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, swapExtent.width, swapExtent.height};
    m_depthImageView = VulkanImageView{m_resources, m_depthImage, VK_IMAGE_ASPECT_DEPTH_BIT};
    m_transferManager.queueTransfer([&] (VulkanCommandBuffer2& cmd) {
        m_depthImage.layoutTransition(cmd, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    });

    m_depthAttachment.imageView = m_depthImageView.get();
    m_depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
}

void VulkanWindowFrameBuffer::doPresentTransition (VulkanCommandBuffer2& cmd) {
    cmd.doImageTransition(m_swapChainImage.image, VK_IMAGE_ASPECT_COLOR_BIT, m_currLayout,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
}

void VulkanWindowFrameBuffer::setClearColor (glm::vec4 color) {
    m_colorAttachment.clearValue.color = {color.r, color.b, color.g, color.a};
}

void VulkanWindowFrameBuffer::prepareRendering (VulkanCommandBuffer2& cmd) {
    if (m_currLayout != VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
        cmd.doImageTransition(m_swapChainImage.image, VK_IMAGE_ASPECT_COLOR_BIT, m_currLayout,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        m_currLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }

    m_colorAttachment.imageLayout = m_currLayout;

    if (m_isFirstRender) {
        m_colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        m_depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        m_isFirstRender = false;
    } else {
        m_colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        m_depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD; // TODO
    }
}

const VkRenderingInfo* VulkanWindowFrameBuffer::getRenderingInfo () const noexcept {
    return &m_renderingInfo;
}
