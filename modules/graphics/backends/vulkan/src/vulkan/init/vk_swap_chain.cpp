#include "vk_swap_chain.h"

using namespace phenyl::vulkan;

static phenyl::Logger LOGGER{"VK_SWAP_CHAIN", detail::VULKAN_LOGGER};

static VkSurfaceFormatKHR ChooseSurfaceFormat (const VulkanSwapChainDetails& details);
static VkPresentModeKHR ChoosePresentMode (const VulkanSwapChainDetails& details);
static VkExtent2D ChooseExtent (const VulkanSwapChainDetails& details);
static std::uint32_t ChooseImageCount (const VulkanSwapChainDetails& details);

VulkanSwapChain::VulkanSwapChain (VkDevice device, VkSurfaceKHR surface, const VulkanSwapChainDetails& details, const VulkanQueueFamilies& queueFamilies) : device{device} {
    PHENYL_LOGI(LOGGER, "Creating swap chain");

    auto surfaceFormat = ChooseSurfaceFormat(details);
    imageFormat = surfaceFormat.format;

    auto presentMode = ChoosePresentMode(details);
    imageExtent = ChooseExtent(details);
    auto imageCount = ChooseImageCount(details);

    std::vector<std::uint32_t> queueFamilyIndices{};
    VkSharingMode sharingMode;
    if (queueFamilies.graphicsFamily != queueFamilies.presentFanily) {
        sharingMode = VK_SHARING_MODE_CONCURRENT;
        queueFamilyIndices = {queueFamilies.graphicsFamily, queueFamilies.presentFanily};
    } else {
        sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    VkSwapchainCreateInfoKHR createInfo{
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = surface,
        .minImageCount = imageCount,
        .imageFormat = surfaceFormat.format,
        .imageColorSpace = surfaceFormat.colorSpace,
        .imageExtent = imageExtent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = sharingMode,
        .queueFamilyIndexCount = static_cast<std::uint32_t>(queueFamilyIndices.size()),
        .pQueueFamilyIndices = !queueFamilyIndices.empty() ? queueFamilyIndices.data() : nullptr,
        .preTransform = details.capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = presentMode,
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE // TODO
    };

    VkSwapchainKHR swapchainKhr;
    auto result = vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapchainKhr);
    PHENYL_ASSERT_MSG(result == VK_SUCCESS, "Failed to create swapchain!");

    swapChain = swapchainKhr;
    createImages();

    PHENYL_LOGI(LOGGER, "Created swap chain");
}

VulkanSwapChain::~VulkanSwapChain () {
    for (auto view : swapChainViews) {
        vkDestroyImageView(device, view, nullptr);
    }

    vkDestroySwapchainKHR(device, swapChain, nullptr);
}

VkViewport VulkanSwapChain::getViewport () const noexcept {
    return VkViewport{
        .x = 0,
        .y = 0,
        .width = static_cast<float>(extent().width),
        .height = static_cast<float>(extent().height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };
}

VkRect2D VulkanSwapChain::getScissor () const noexcept {
    return VkRect2D{
        .offset = {0, 0},
        .extent = extent()
    };
}


SwapChainImage VulkanSwapChain::acquireImage (const VulkanSemaphore& signalSem) {
    auto result = vkAcquireNextImageKHR(device, swapChain, std::numeric_limits<std::uint64_t>::max(),
        signalSem.get(), VK_NULL_HANDLE, &currIndex);
    PHENYL_ASSERT_MSG(result == VK_SUCCESS, "Failed to acquire image: {}", result);

    return SwapChainImage{
        .image = swapChainImages.at(currIndex),
        .view = swapChainViews.at(currIndex)
    };
}

void VulkanSwapChain::present (VkQueue queue, const VulkanSemaphore& waitSem) {
    VkSemaphore sem = waitSem.get();

    VkPresentInfoKHR presentInfo{
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &sem,
        .swapchainCount = 1,
        .pSwapchains = &swapChain,
        .pImageIndices = &currIndex
    };

    auto result = vkQueuePresentKHR(queue, &presentInfo);
    if (result != VK_SUCCESS) {
        PHENYL_LOGE(LOGGER, "Failed to present swapchain image: {}", result);
    }
}

void VulkanSwapChain::createImages () {
    swapChainImages = Enumerate<VkImage>(vkGetSwapchainImagesKHR, device, swapChain);
    swapChainViews.reserve(swapChainImages.size());

    // TODO: abstract away into class
    for (const auto& image : swapChainImages) {
        VkComponentMapping components{
            .r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .a = VK_COMPONENT_SWIZZLE_IDENTITY
        };

        VkImageSubresourceRange subresourceRange{
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        };

        VkImageViewCreateInfo createInfo{
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = image,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = imageFormat,
            .components = components,
            .subresourceRange = subresourceRange
        };

        VkImageView imageView;
        auto result = vkCreateImageView(device, &createInfo, nullptr, &imageView);
        PHENYL_ASSERT_MSG(result == VK_SUCCESS, "Failed to create image view!");

        swapChainViews.emplace_back(imageView);
    }

    PHENYL_LOGI(LOGGER, "Created swap chain images");
}

static VkSurfaceFormatKHR ChooseSurfaceFormat (const VulkanSwapChainDetails& details) {
    PHENYL_DASSERT(!details.formats.empty());

    for (const auto i : details.formats) {
        PHENYL_LOGD(LOGGER, "Found available surface format: {} with color space: {}", string_VkFormat(i.format), string_VkColorSpaceKHR(i.colorSpace));
    }

    for (const auto& surfaceFormat : details.formats) {
        if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB && surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            PHENYL_LOGI(LOGGER, "Found preferred SRGB surface format");
            return surfaceFormat;
        }
    }

    auto usedFormat = details.formats.front();
    PHENYL_LOGI(LOGGER, "Could not find preferred surface format, using {} format with {} color space",
        string_VkFormat(usedFormat.format), string_VkColorSpaceKHR(usedFormat.colorSpace));
    return usedFormat;
}

static VkPresentModeKHR ChoosePresentMode (const VulkanSwapChainDetails& details) {
    // TODO: vsync settings
    for (const auto i : details.presentModes) {
        PHENYL_LOGD(LOGGER, "Found available present mode: {}", string_VkPresentModeKHR(i));
    }

    if (auto it = std::ranges::find(details.presentModes, VK_PRESENT_MODE_MAILBOX_KHR); it != details.presentModes.end()) {
        PHENYL_LOGI(LOGGER, "Found {} present mode", string_VkPresentModeKHR(VK_PRESENT_MODE_MAILBOX_KHR));
        return VK_PRESENT_MODE_MAILBOX_KHR;
    }

    PHENYL_LOGI(LOGGER, "Falling back to {} present mode", string_VkPresentModeKHR(VK_PRESENT_MODE_FIFO_KHR));
    return VK_PRESENT_MODE_FIFO_KHR;
}

static VkExtent2D ChooseExtent (const VulkanSwapChainDetails& details) {
    if (details.capabilities.currentExtent.width != std::numeric_limits<std::uint32_t>::max()) {
        return details.capabilities.currentExtent;
    } else {
        PHENYL_ABORT("Extent lookup unimplemented!");
    }
}

std::uint32_t ChooseImageCount (const VulkanSwapChainDetails& details) {
    auto preferredImageCount = details.capabilities.minImageCount + 1;

    return details.capabilities.maxImageCount ? std::min(preferredImageCount, details.capabilities.maxImageCount) : preferredImageCount;
}
