#include "vk_device.h"

#include <algorithm>
#include <cstring>
#include <ranges>
#include <unordered_set>

using namespace phenyl::vulkan;

static phenyl::Logger LOGGER{"VK_DEVICE", detail::VULKAN_LOGGER};

VulkanDevice::VulkanDevice (VkInstance instance, VkSurfaceKHR surface) {
    std::vector deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME};

    choosePhysicalDevice(instance, surface, deviceExtensions);
    m_logicalDevice = createLogicalDevice(deviceExtensions);
    PHENYL_DASSERT(m_logicalDevice);

    // vkGetDeviceQueue(logicalDevice, families.graphicsFamily, 0, &graphicsQueue);
    // vkGetDeviceQueue(logicalDevice, families.presentFanily, 0, &presentQueue);
    m_graphicsQueue = makeQueue(m_queueFamilies.graphicsFamily);
    m_presentQueue = makeQueue(m_queueFamilies.presentFanily);

    PHENYL_LOGI(LOGGER, "Created logical device with 2 queues");
}

std::unique_ptr<VulkanSwapChain> VulkanDevice::makeSwapChain (VulkanResources& resources, VkSurfaceKHR surface) {
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physicalDevice, surface, &m_swapChainDetails.capabilities);
    return std::make_unique<VulkanSwapChain>(resources, surface, m_swapChainDetails, m_queueFamilies);
}

VkCommandPool VulkanDevice::makeCommandPool (VkCommandPoolCreateFlags usage) {
    VkCommandPoolCreateInfo createInfo{.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .flags = usage,
      .queueFamilyIndex = m_queueFamilies.graphicsFamily};
    VkCommandPool pool = nullptr;
    auto result = vkCreateCommandPool(m_logicalDevice, &createInfo, nullptr, &pool);
    PHENYL_LOGE_IF(result != VK_SUCCESS, LOGGER, "Failed to create VkCommandPool: {}", result);

    return pool;
}

VmaAllocator VulkanDevice::makeVmaAllocator (VkInstance instance, std::uint32_t vkVersion) {
    VmaAllocatorCreateInfo allocatorInfo{.physicalDevice = m_physicalDevice,
      .device = m_logicalDevice,
      .instance = instance,
      .vulkanApiVersion = vkVersion};

    VmaAllocator allocator;
    auto result = vmaCreateAllocator(&allocatorInfo, &allocator);
    PHENYL_ASSERT_MSG(result == VK_SUCCESS, "Failed to create VMA allocator: {}", result);
    PHENYL_DASSERT(allocator);
    return allocator;
}

VulkanDevice::~VulkanDevice () {
    vkDestroyDevice(m_logicalDevice, nullptr);
}

void VulkanDevice::choosePhysicalDevice (VkInstance instance, VkSurfaceKHR surface,
    const std::vector<const char*>& deviceExtensions) {
    auto devices = Enumerate<VkPhysicalDevice>(vkEnumeratePhysicalDevices, instance);
    PHENYL_ASSERT_MSG(!devices.empty(), "Failed to find physical devices with Vulkan support");

    for (auto device : devices) {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        PHENYL_LOGI(LOGGER, "Found physical device \"{}\" (API version: {}, driver version: {})",
            deviceProperties.deviceName, VulkanVersion::FromPacked(deviceProperties.apiVersion),
            deviceProperties.driverVersion);
    }

    // VulkanQueueFamilies queueFamilies{};
    for (auto device : devices) {
        if (!CheckDeviceExtensionSupport(device, deviceExtensions)) {
            continue;
        }

        if (!CheckDeviceFeatures(device)) {
            continue;
        }

        auto familes = GetDeviceFamilies(device, surface);
        if (!familes) {
            continue;
        }

        auto swapDetails = GetDeviceSwapChainDetails(device, surface);
        if (!swapDetails) {
            continue;
        }

        m_physicalDevice = device;
        m_queueFamilies = *familes;
        m_swapChainDetails = std::move(*swapDetails);
        break;
    }

    PHENYL_ASSERT_MSG(m_physicalDevice, "Failed to find a suitable physical device!");

    m_properties = GetDeviceProperties(m_physicalDevice);
    PHENYL_LOGI(LOGGER, "Chose physical device \"{}\"", properties().deviceName);
}

VkDevice VulkanDevice::createLogicalDevice (const std::vector<const char*>& deviceExtensions) {
    PHENYL_DASSERT(m_physicalDevice);

    std::unordered_set familyIndexes{m_queueFamilies.graphicsFamily, m_queueFamilies.presentFanily};
    float priority = 1.0f;
    auto queueCreateInfos = familyIndexes | std::views::transform([&] (auto i) {
        return VkDeviceQueueCreateInfo{.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
          .queueFamilyIndex = i,
          .queueCount = 1,
          .pQueuePriorities = &priority};
    }) | std::ranges::to<std::vector>();

    VkPhysicalDeviceVulkan13Features vulkan13Features{
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
      .synchronization2 = true,
      .dynamicRendering = true,
    };

    VkPhysicalDeviceFeatures2 deviceFeatures2{
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
      .pNext = &vulkan13Features,
      .features = {.samplerAnisotropy = true},
    };

    VkDeviceCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .pNext = &deviceFeatures2,
      .queueCreateInfoCount = static_cast<std::uint32_t>(queueCreateInfos.size()),
      .pQueueCreateInfos = queueCreateInfos.data(),
      .enabledExtensionCount = static_cast<std::uint32_t>(deviceExtensions.size()),
      .ppEnabledExtensionNames = deviceExtensions.data(),
    };

    VkDevice device;
    if (auto result = vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &device); result != VK_SUCCESS) {
        PHENYL_ABORT("Failed to create logical device (error: {})", result);
    }

    return device;
}

VkQueue VulkanDevice::makeQueue (std::uint32_t queueFamilyIndex) {
    VkQueue queue;
    vkGetDeviceQueue(m_logicalDevice, queueFamilyIndex, 0, &queue);
    PHENYL_ASSERT(queue);

    return queue;
}

std::optional<VulkanQueueFamilies> VulkanDevice::GetDeviceFamilies (VkPhysicalDevice device, VkSurfaceKHR surface) {
    constexpr std::uint32_t REQUIRED_NUM_FAMILIES = 2;

    std::uint32_t queueFamilyCount = 0;
    std::optional<std::uint32_t> graphicsFamily;
    std::optional<std::uint32_t> presentFamily;

    std::uint32_t index = 0;
    for (const auto& props : Enumerate<VkQueueFamilyProperties>(vkGetPhysicalDeviceQueueFamilyProperties, device)) {
        if (!graphicsFamily && props.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphicsFamily = index;
            queueFamilyCount++;
        }

        if (!presentFamily) {
            VkBool32 presentSupport = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, index, surface, &presentSupport);

            if (presentSupport) {
                presentFamily = index;
                queueFamilyCount++;
            }
        }

        if (queueFamilyCount == REQUIRED_NUM_FAMILIES) {
            break;
        }
        index++;
    }

    if (queueFamilyCount == REQUIRED_NUM_FAMILIES) {
        return VulkanQueueFamilies{.graphicsFamily = *graphicsFamily, .presentFanily = *presentFamily};
    }
    return std::nullopt;
}

bool VulkanDevice::CheckDeviceExtensionSupport (VkPhysicalDevice device, const std::vector<const char*>& extensions) {
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    auto deviceExtensions = Enumerate<VkExtensionProperties>(vkEnumerateDeviceExtensionProperties, device, nullptr);
    bool allPresent = true;
    for (auto i : extensions) {
        bool present = std::ranges::any_of(deviceExtensions,
            [&] (const VkExtensionProperties& x) { return std::strcmp(i, x.extensionName) == 0; });

        if (!present) {
            PHENYL_LOGD(detail::VULKAN_LOGGER, "Extension {} not supported by physical device \"{}\"", i,
                deviceProperties.deviceName);
            allPresent = false;
        }
    }

    return allPresent;
}

std::optional<VulkanSwapChainDetails> VulkanDevice::GetDeviceSwapChainDetails (VkPhysicalDevice device,
    VkSurfaceKHR surface) {
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &capabilities);

    auto formats = Enumerate<VkSurfaceFormatKHR>(vkGetPhysicalDeviceSurfaceFormatsKHR, device, surface);
    auto presentModes = Enumerate<VkPresentModeKHR>(vkGetPhysicalDeviceSurfacePresentModesKHR, device, surface);

    if (formats.empty() || presentModes.empty()) {
        return std::nullopt;
    }

    return VulkanSwapChainDetails{.capabilities = capabilities,
      .formats = std::move(formats),
      .presentModes = std::move(presentModes)};
}

bool VulkanDevice::CheckDeviceFeatures (VkPhysicalDevice device) {
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    VkPhysicalDeviceVulkan13Features vk13Features{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};

    VkPhysicalDeviceFeatures2 features2{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, .pNext = &vk13Features};
    vkGetPhysicalDeviceFeatures2(device, &features2);

    const auto& features = features2.features;

    // if (!vk13Features) {
    //     PHENYL_LOGD(LOGGER, "Physical device \"{}\" missing Vulkan 1.3 features",
    //     deviceProperties.deviceName); return false;
    // }

    if (!features.samplerAnisotropy) {
        PHENYL_LOGD(LOGGER, "Physical device \"{}\" missing feature samplerAnisotropy", deviceProperties.deviceName);
        return false;
    }

    if (!vk13Features.dynamicRendering) {
        PHENYL_LOGD(LOGGER, "Physical device \"{}\" missing feature dynamicRendering", deviceProperties.deviceName);
        return false;
    }

    if (!vk13Features.synchronization2) {
        PHENYL_LOGD(LOGGER, "Physical device \"{}\" missing feature synchronization2", deviceProperties.deviceName);
        return false;
    }

    return true;
}

DeviceProperties VulkanDevice::GetDeviceProperties (VkPhysicalDevice device) {
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    return {.deviceName = deviceProperties.deviceName,
      .maxAnisotropy = deviceProperties.limits.maxSamplerAnisotropy,
      .minUniformAlignment = deviceProperties.limits.minUniformBufferOffsetAlignment};
}
