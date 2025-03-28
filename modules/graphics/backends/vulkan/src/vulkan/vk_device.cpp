#include "vk_device.h"

using namespace phenyl::vulkan;

static phenyl::Logger LOGGER{"DEVICE", detail::VULKAN_LOGGER};

VulkanDevice::VulkanDevice (VkInstance instance) {
    auto families = choosePhysicalDevice(instance);
    logicalDevice = createLogicalDevice(families);
    PHENYL_DASSERT(logicalDevice);

    vkGetDeviceQueue(logicalDevice, families.graphicsFamily, 0, &graphicsQueue);
}

VulkanDevice::~VulkanDevice () {
    vkDestroyDevice(logicalDevice, nullptr);
}

VulkanQueueFamilies VulkanDevice::choosePhysicalDevice (VkInstance instance) {
    auto devices = Enumerate<VkPhysicalDevice>(vkEnumeratePhysicalDevices, instance);
    PHENYL_ASSERT_MSG(!devices.empty(), "Failed to find physical devices with Vulkan support");

    for (auto device : devices) {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        PHENYL_LOGI(LOGGER, "Found physical device \"{}\" (API version: {}, driver version: {})", deviceProperties.deviceName,
            VulkanVersion::FromPacked(deviceProperties.apiVersion), deviceProperties.driverVersion);
    }


    VulkanQueueFamilies queueFamilies{};
    for (auto device : devices) {
        auto familes = GetDeviceFamilies(device);
        if (familes) {
            physicalDevice = device;
            queueFamilies = *familes;
            break;
        }
    }

    PHENYL_ASSERT_MSG(physicalDevice, "Failed to find a suitable physical device!");

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
    PHENYL_LOGI(LOGGER, "Chose physical device \"{}\"", deviceProperties.deviceName);
    return queueFamilies;
}

VkDevice VulkanDevice::createLogicalDevice (const VulkanQueueFamilies& queueFamilies) {
    PHENYL_DASSERT(physicalDevice);

    float priority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo{
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = queueFamilies.graphicsFamily,
        .queueCount = 1,
        .pQueuePriorities = &priority
    };

    // TODO
    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queueCreateInfo,
        .pEnabledFeatures = &deviceFeatures
    };

    VkDevice device;
    if (auto result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &device); result != VK_SUCCESS) {
        PHENYL_ABORT("Failed to create logical device (error: {})", result);
    }

    return device;
}

std::optional<VulkanQueueFamilies> VulkanDevice::GetDeviceFamilies (VkPhysicalDevice device) {
    std::uint32_t queueFamilyCount = 0;
    std::optional<std::uint32_t> graphicsFamily;

    std::uint32_t index = 0;
    for (const auto& properties : Enumerate<VkQueueFamilyProperties>(vkGetPhysicalDeviceQueueFamilyProperties, device)) {
        if (!graphicsFamily && properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphicsFamily = index;
            queueFamilyCount++;
        }

        if (queueFamilyCount == 1) {
            break;
        }
        index++;
    }

    if (graphicsFamily) {
        return VulkanQueueFamilies{
            .graphicsFamily = *graphicsFamily
        };
    }
    return std::nullopt;
}
