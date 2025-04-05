#include "vk_device.h"
#include <cstring>
#include <ranges>
#include <unordered_set>
#include <algorithm>

using namespace phenyl::vulkan;

static phenyl::Logger LOGGER{"VK_DEVICE", detail::VULKAN_LOGGER};

VulkanDevice::VulkanDevice (VkInstance instance, VkSurfaceKHR surface) {
    std::vector deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
    };

    choosePhysicalDevice(instance, surface, deviceExtensions);
    logicalDevice = createLogicalDevice(deviceExtensions);
    PHENYL_DASSERT(logicalDevice);

    // vkGetDeviceQueue(logicalDevice, families.graphicsFamily, 0, &graphicsQueue);
    // vkGetDeviceQueue(logicalDevice, families.presentFanily, 0, &presentQueue);
    graphicsQueue = makeQueue(queueFamilies.graphicsFamily);
    presentQueue = makeQueue(queueFamilies.presentFanily);

    PHENYL_LOGI(LOGGER, "Created logical device with 2 queues");
}

std::unique_ptr<VulkanSwapChain> VulkanDevice::makeSwapChain (VkSurfaceKHR surface) {
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &swapChainDetails.capabilities);
    return std::make_unique<VulkanSwapChain>(logicalDevice, surface, swapChainDetails, queueFamilies);
}

VulkanCommandPool VulkanDevice::makeCommandPool (std::size_t initialCapacity) {
    return VulkanCommandPool{logicalDevice, queueFamilies.graphicsFamily, initialCapacity};
}

VmaAllocator VulkanDevice::makeVmaAllocator (VkInstance instance, std::uint32_t vkVersion) {
    VmaAllocatorCreateInfo allocatorInfo{
        .physicalDevice = physicalDevice,
        .device = logicalDevice,
        .instance = instance,
        .vulkanApiVersion = vkVersion
    };

    VmaAllocator allocator;
    auto result = vmaCreateAllocator(&allocatorInfo, &allocator);
    PHENYL_ASSERT_MSG(result == VK_SUCCESS, "Failed to create VMA allocator: {}", result);
    PHENYL_DASSERT(allocator);
    return allocator;
}

VulkanDevice::~VulkanDevice () {
    vkDestroyDevice(logicalDevice, nullptr);
}

void VulkanDevice::choosePhysicalDevice (VkInstance instance, VkSurfaceKHR surface, const std::vector<const char*>& deviceExtensions) {
    auto devices = Enumerate<VkPhysicalDevice>(vkEnumeratePhysicalDevices, instance);
    PHENYL_ASSERT_MSG(!devices.empty(), "Failed to find physical devices with Vulkan support");

    for (auto device : devices) {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        PHENYL_LOGI(LOGGER, "Found physical device \"{}\" (API version: {}, driver version: {})", deviceProperties.deviceName,
            VulkanVersion::FromPacked(deviceProperties.apiVersion), deviceProperties.driverVersion);
    }

    //VulkanQueueFamilies queueFamilies{};
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

        physicalDevice = device;
        queueFamilies = *familes;
        swapChainDetails = std::move(*swapDetails);
        break;
    }

    PHENYL_ASSERT_MSG(physicalDevice, "Failed to find a suitable physical device!");

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
    PHENYL_LOGI(LOGGER, "Chose physical device \"{}\"", deviceProperties.deviceName);
}

VkDevice VulkanDevice::createLogicalDevice (const std::vector<const char*>& deviceExtensions) {
    PHENYL_DASSERT(physicalDevice);

    std::unordered_set familyIndexes{queueFamilies.graphicsFamily, queueFamilies.presentFanily};
    float priority = 1.0f;
    auto queueCreateInfos = familyIndexes
        | std::views::transform([&] (auto i) {
                return VkDeviceQueueCreateInfo{
                    .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                    .queueFamilyIndex = i,
                    .queueCount = 1,
                    .pQueuePriorities = &priority
                };
            })
        | std::ranges::to<std::vector>();

    VkPhysicalDeviceVulkan13Features vulkan13Features{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
        .synchronization2 = true,
        .dynamicRendering = true,
    };

    VkPhysicalDeviceFeatures2 deviceFeatures2{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
        .pNext = &vulkan13Features
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
    if (auto result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &device); result != VK_SUCCESS) {
        PHENYL_ABORT("Failed to create logical device (error: {})", result);
    }

    return device;
}

VkQueue VulkanDevice::makeQueue (std::uint32_t queueFamilyIndex) {
    VkQueue queue;
    vkGetDeviceQueue(logicalDevice, queueFamilyIndex, 0, &queue);
    PHENYL_ASSERT(queue);

    return queue;
}


std::optional<VulkanQueueFamilies> VulkanDevice::GetDeviceFamilies (VkPhysicalDevice device, VkSurfaceKHR surface) {
    constexpr std::uint32_t REQUIRED_NUM_FAMILIES = 2;

    std::uint32_t queueFamilyCount = 0;
    std::optional<std::uint32_t> graphicsFamily;
    std::optional<std::uint32_t> presentFamily;

    std::uint32_t index = 0;
    for (const auto& properties : Enumerate<VkQueueFamilyProperties>(vkGetPhysicalDeviceQueueFamilyProperties, device)) {
        if (!graphicsFamily && properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
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
        return VulkanQueueFamilies{
            .graphicsFamily = *graphicsFamily,
            .presentFanily = *presentFamily
        };
    }
    return std::nullopt;
}

bool VulkanDevice::CheckDeviceExtensionSupport (VkPhysicalDevice device, const std::vector<const char*>& extensions) {
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    auto deviceExtensions = Enumerate<VkExtensionProperties>(vkEnumerateDeviceExtensionProperties, device, nullptr);
    bool allPresent = true;
    for (auto i : extensions) {
        bool present = std::ranges::any_of(deviceExtensions, [&] (const VkExtensionProperties& x) {
            return std::strcmp(i, x.extensionName) == 0;
        });

        if (!present) {
            PHENYL_LOGD(detail::VULKAN_LOGGER, "Extension {} not supported by physical device \"{}\"", i, deviceProperties.deviceName);
            allPresent = false;
        }
    }

    return allPresent;
}

std::optional<VulkanSwapChainDetails> VulkanDevice::GetDeviceSwapChainDetails (VkPhysicalDevice device, VkSurfaceKHR surface) {
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &capabilities);

    auto formats = Enumerate<VkSurfaceFormatKHR>(vkGetPhysicalDeviceSurfaceFormatsKHR, device, surface);
    auto presentModes = Enumerate<VkPresentModeKHR>(vkGetPhysicalDeviceSurfacePresentModesKHR,
        device, surface);

    if (formats.empty() || presentModes.empty()) {
        return std::nullopt;
    }

    return VulkanSwapChainDetails{
        .capabilities = capabilities,
        .formats = std::move(formats),
        .presentModes = std::move(presentModes)
    };
}

bool VulkanDevice::CheckDeviceFeatures (VkPhysicalDevice device) {
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    VkPhysicalDeviceVulkan13Features vk13Features{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES
    };

    VkPhysicalDeviceFeatures2 features2{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
        .pNext = &vk13Features
    };
    vkGetPhysicalDeviceFeatures2(device, &features2);

    auto* features = &features2.features;

    // if (!vk13Features) {
    //     PHENYL_LOGD(LOGGER, "Physical device \"{}\" missing Vulkan 1.3 features", deviceProperties.deviceName);
    //     return false;
    // }

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
