#include "vk_framebuffer.h"

using namespace phenyl::vulkan;

void VulkanFrameBuffer::prepareRendering (VulkanCommandBuffer2& cmd) {

}

const VkRenderingInfo* VulkanFrameBuffer::getRenderingInfo () const noexcept {
    return nullptr;
}

VkViewport VulkanFrameBuffer::viewport () const noexcept {
    return {};
}

VkRect2D VulkanFrameBuffer::scissor () const noexcept {
    return {};
}

void VulkanFrameBuffer::clear (glm::vec4 clearColor) {

}

phenyl::graphics::ISampler* VulkanFrameBuffer::getSampler () noexcept {
    return &dummy;
}

phenyl::graphics::ISampler* VulkanFrameBuffer::getDepthSampler () noexcept {
    return &dummy;
}

glm::ivec2 VulkanFrameBuffer::getDimensions () const noexcept {
    return {0, 0};
}

VulkanWindowFrameBuffer::VulkanWindowFrameBuffer (VulkanResources& resources, TransferManager& transferManager) : resources{resources}, transferManager{transferManager} {
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

    depthImage = VulkanImage{resources, VK_FORMAT_D24_UNORM_S8_UINT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, swapExtent.width, swapExtent.height};
    depthImageView = VulkanImageView{resources, depthImage, VK_IMAGE_ASPECT_DEPTH_BIT};
    transferManager.queueTransfer([&] (VulkanCommandBuffer2& cmd) {
        depthImage.layoutTransition(cmd, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    });

    depthAttachment.imageView = depthImageView.get();
    depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
}

void VulkanWindowFrameBuffer::doPresentTransition (VulkanCommandBuffer2& cmd) {
    cmd.doImageTransition(swapChainImage.image, currLayout, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
}

void VulkanWindowFrameBuffer::setClearColor (glm::vec4 color) {
    colorAttachment.clearValue.color = {color.r, color.b, color.g, color.a};
}

void VulkanWindowFrameBuffer::prepareRendering (VulkanCommandBuffer2& cmd) {
    if (currLayout != VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
        cmd.doImageTransition(swapChainImage.image, currLayout, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
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
