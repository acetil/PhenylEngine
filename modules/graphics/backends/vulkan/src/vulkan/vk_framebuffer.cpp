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
    return VkPipelineRenderingCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .colorAttachmentCount = static_cast<std::uint32_t>(colorFormats.size()),
        .pColorAttachmentFormats = colorFormats.data(),
        .depthAttachmentFormat = depthFormat,
        .stencilAttachmentFormat = stencilFormat
    };
}

FrameBufferLayoutManager::FrameBufferLayoutManager () : layoutCache{} {}

const FrameBufferLayout* FrameBufferLayoutManager::cacheLayout (FrameBufferLayout&& layout) {
    auto it = layoutCache.find(layout);
    if (it != layoutCache.end()) {
        return &*it;
    }

    return &*layoutCache.emplace(std::move(layout)).first;
}

VulkanFrameBuffer::VulkanFrameBuffer (VulkanResources& resources, FrameBufferLayoutManager& layoutManager,
    const graphics::FrameBufferProperties& properties, std::uint32_t width, std::uint32_t height) : IVulkanFrameBuffer{layoutManager.cacheLayout(GetFrameBufferLayout(properties))}, extent{width, height} {
    const auto* fbLayout = layout();

    for (auto colorFormat : fbLayout->colorFormats) {
        colorSamplers.emplace_back(resources, properties);
        colorSamplers.back().recreate(resources, VulkanImage{resources, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, width, height});

        colorAttachments.emplace_back(VkRenderingAttachmentInfo{
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = colorSamplers.back().view(),
            .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .resolveMode = VK_RESOLVE_MODE_NONE,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE
        });
    }

    if (properties.depthFormat) {
        depthSampler = CombinedSampler{resources, properties, VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL};
        depthSampler->recreate(resources, VulkanImage{resources, fbLayout->depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, width, height}, VK_IMAGE_ASPECT_DEPTH_BIT);

        depthAttachment = VkRenderingAttachmentInfo{
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = depthSampler->view(),
            .imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            .resolveMode = VK_RESOLVE_MODE_NONE,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE
        };
    }

    renderingInfo = VkRenderingInfo{
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .renderArea = VkRect2D{
            .offset = {0, 0},
            .extent = extent
        },
        .layerCount = 1,
        .colorAttachmentCount = static_cast<std::uint32_t>(colorAttachments.size()),
        .pColorAttachments = colorAttachments.data(),
        .pDepthAttachment = depthSampler ? &depthAttachment : nullptr,
    };
}

void VulkanFrameBuffer::prepareRendering (VulkanCommandBuffer2& cmd) {
    for (std::size_t i = 0; i < colorSamplers.size(); i++) {
        colorSamplers[i].image().layoutTransition(cmd, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        if (clearColor) {
            colorAttachments[i].clearValue = VkClearValue{
                .color = *clearColor
            };
            colorAttachments[i].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        } else {
            colorAttachments[i].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        }
    }

    if (depthSampler) {
        depthSampler->image().layoutTransition(cmd, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

        if (clearColor) {
            depthAttachment.clearValue = VkClearValue{
                .depthStencil = VkClearDepthStencilValue{
                    .depth = 1.0f
                }
            };
            depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        } else {
            depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        }
    }

    clearColor = std::nullopt;
}

const VkRenderingInfo* VulkanFrameBuffer::getRenderingInfo () const noexcept {
    return &renderingInfo;
}

VkViewport VulkanFrameBuffer::viewport () const noexcept {
    return VkViewport{
        .x = 0,
        .y = 0,
        .width = static_cast<float>(extent.width),
        .height = static_cast<float>(extent.height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };
}

VkRect2D VulkanFrameBuffer::scissor () const noexcept {
    return VkRect2D{
        .offset = {0, 0},
        .extent = extent
    };
}

void VulkanFrameBuffer::clear (glm::vec4 clearColor) {
    this->clearColor = VkClearColorValue{
        .float32 = {clearColor.r, clearColor.g, clearColor.b, clearColor.a}
    };
}

phenyl::graphics::ISampler* VulkanFrameBuffer::getSampler () noexcept {
    return !colorSamplers.empty() ? &colorSamplers[0] : nullptr;
}

phenyl::graphics::ISampler* VulkanFrameBuffer::getDepthSampler () noexcept {
    return depthSampler ? &*depthSampler : nullptr;
}

glm::ivec2 VulkanFrameBuffer::getDimensions () const noexcept {
    return {extent.width, extent.height};
}

VulkanWindowFrameBuffer::VulkanWindowFrameBuffer (VulkanResources& resources, TransferManager& transferManager, FrameBufferLayoutManager& layoutManager, VkFormat colorFormat) :
    IVulkanFrameBuffer{layoutManager.cacheLayout(FrameBufferLayout{
        .colorFormats = {colorFormat},
        .depthFormat = VK_FORMAT_D24_UNORM_S8_UINT
    })}, resources{resources}, transferManager{transferManager}  {
    colorAttachment = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .resolveMode = VK_RESOLVE_MODE_NONE,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue = {
            .color = {0, 0, 0, 0}
        }
    };

    depthAttachment = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .resolveMode = VK_RESOLVE_MODE_NONE,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE, // TODO
        .clearValue = {
            .depthStencil = {
                .depth = 1.0f
            }
        }
    };

    renderingInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachment,
        .pDepthAttachment = &depthAttachment

        // TODO: stencil
    };
}

VkViewport VulkanWindowFrameBuffer::viewport () const noexcept {
    PHENYL_ASSERT(swapChain);

    auto extent = swapChain->extent();
    return VkViewport{
        .x = 0,
        .y = 0,
        .width = static_cast<float>(extent.width),
        .height = static_cast<float>(extent.height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };
}

VkRect2D VulkanWindowFrameBuffer::scissor () const noexcept {
    return VkRect2D{
        .offset = {0, 0},
        .extent = swapChain->extent()
    };
}

bool VulkanWindowFrameBuffer::acquireFrame (const VulkanSemaphore& waitSem) {
    PHENYL_ASSERT(swapChain);

    auto image = swapChain->acquireImage(waitSem);
    if (!image) {
        return false;
    }

    swapChainImage = *image;
    currLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    colorAttachment.imageView = swapChainImage.view;
    isFirstRender = true;

    return true;
}

void VulkanWindowFrameBuffer::onSwapChainRecreate (VulkanSwapChain* newSwapChain) {
    PHENYL_ASSERT(newSwapChain);

    swapChain = newSwapChain;

    renderingInfo.renderArea = {
        .offset = {0, 0},
        .extent = swapChain->extent()
    };

    auto swapExtent = swapChain->extent();
    if (swapExtent.width == depthImage.width() && swapExtent.height == depthImage.height()) {
        // No need to change depth buffer
        return;
    }

    depthImage = VulkanImage{resources, VK_FORMAT_D24_UNORM_S8_UINT, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, swapExtent.width, swapExtent.height};
    depthImageView = VulkanImageView{resources, depthImage, VK_IMAGE_ASPECT_DEPTH_BIT};
    transferManager.queueTransfer([&] (VulkanCommandBuffer2& cmd) {
        depthImage.layoutTransition(cmd, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    });

    depthAttachment.imageView = depthImageView.get();
    depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
}

void VulkanWindowFrameBuffer::doPresentTransition (VulkanCommandBuffer2& cmd) {
    cmd.doImageTransition(swapChainImage.image, VK_IMAGE_ASPECT_COLOR_BIT, currLayout, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
}

void VulkanWindowFrameBuffer::setClearColor (glm::vec4 color) {
    colorAttachment.clearValue.color = {color.r, color.b, color.g, color.a};
}

void VulkanWindowFrameBuffer::prepareRendering (VulkanCommandBuffer2& cmd) {
    if (currLayout != VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
        cmd.doImageTransition(swapChainImage.image, VK_IMAGE_ASPECT_COLOR_BIT, currLayout, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        currLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }

    colorAttachment.imageLayout = currLayout;

    if (isFirstRender) {
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        isFirstRender = false;
    } else {
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD; // TODO
    }
}

const VkRenderingInfo* VulkanWindowFrameBuffer::getRenderingInfo () const noexcept {
    return &renderingInfo;
}
