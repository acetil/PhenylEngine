#include <cstring>
#include <ranges>

#include "vk_renderer.h"

#include <unordered_set>

using namespace phenyl::graphics;
using namespace phenyl::vulkan;

phenyl::Logger phenyl::vulkan::detail::VULKAN_LOGGER{"VULKAN", phenyl::PHENYL_LOGGER};

namespace phenyl::vulkan {
    class VulkanViewport : public glfw::GLFWViewport {
    private:
        static void VulkanWindowHints () {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

            // TODO
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        }
    public:
        explicit VulkanViewport (const GraphicsProperties& properties) : GLFWViewport{properties, VulkanWindowHints} {}

        [[nodiscard]] std::vector<const char*> getGLFWExtensions () const noexcept {
            std::uint32_t extensionNum = 0;
            auto** extensions = glfwGetRequiredInstanceExtensions(&extensionNum);

            return {extensions, extensions + extensionNum};
        }
    };

    struct DebugMessage {
        VkDebugUtilsMessageSeverityFlagBitsEXT severity;
        VkDebugUtilsMessageTypeFlagsEXT type;
        const VkDebugUtilsMessengerCallbackDataEXT* data;
    };
}

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugMessageCallback (VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

VulkanRenderer::VulkanRenderer (const GraphicsProperties& properties, std::unique_ptr<VulkanViewport> vkViewport) : viewport{std::move(vkViewport)} {
    PHENYL_DASSERT(viewport);


    createVkInstance(properties);
    PHENYL_DASSERT(instance);

    setupDebugMessenger();
}

VulkanRenderer::~VulkanRenderer () {
    destroyDebugMessenger();

    vkDestroyInstance(instance, nullptr);

    viewport = nullptr;
}

double VulkanRenderer::getCurrentTime () {
    return viewport->getTime();
}

void VulkanRenderer::clearWindow () {

}

void VulkanRenderer::render () {

}

void VulkanRenderer::finishRender () {

}

Viewport& VulkanRenderer::getViewport () {
    return *viewport;
}

const Viewport& VulkanRenderer::getViewport () const {
    return *viewport;
}

std::string_view VulkanRenderer::getName () const noexcept {
    return "VulkanRenderer";
}

std::unique_ptr<IBuffer> VulkanRenderer::makeRendererBuffer (std::size_t startCapacity, std::size_t elementSize) {
    return nullptr;
}

std::unique_ptr<IUniformBuffer> VulkanRenderer::makeRendererUniformBuffer (bool readable) {
    return nullptr;
}

std::unique_ptr<IImageTexture> VulkanRenderer::makeRendererImageTexture (const TextureProperties& properties) {
    return nullptr;
}

std::unique_ptr<IImageArrayTexture> VulkanRenderer::makeRendererArrayTexture (const TextureProperties& properties, std::uint32_t width, std::uint32_t height) {
    return nullptr;
}

std::unique_ptr<IFrameBuffer> VulkanRenderer::makeRendererFrameBuffer (const FrameBufferProperties& properties, std::uint32_t width, std::uint32_t height) {
    return nullptr;
}

PipelineBuilder VulkanRenderer::buildPipeline () {
    return PipelineBuilder{nullptr};
}

void VulkanRenderer::loadDefaultShaders () {}

void VulkanRenderer::onDebugMessage (const DebugMessage& msg) {
    auto* messageIdName = msg.data->pMessageIdName;
    auto* message = msg.data->pMessage;

    std::string_view type;
    if (msg.type == VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) {
        type = "GENERAL";
    } else if (msg.type == VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
        type = "VALIDATION";
    } else if (msg.type == VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
        type = "PERFORMANCE";
    } else if (msg.type == VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT) {
        type = "DEVICE_ADDRESS_BINDING";
    } else {
        PHENYL_LOGE(detail::VULKAN_LOGGER, "Received unexpected message type {}", msg.type);
        type = "UNEXPECTED";
    }

    if (msg.severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        PHENYL_LOGE(detail::VULKAN_LOGGER, "Vulkan {} message ({}): \"{}\"", type, messageIdName, message);
    } else if (msg.severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        PHENYL_LOGW(detail::VULKAN_LOGGER, "Vulkan {} message ({}): \"{}\"", type, messageIdName, message);
    } else if (msg.severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        PHENYL_LOGI(detail::VULKAN_LOGGER, "Vulkan {} message ({}): \"{}\"", type, messageIdName, message);
    } else if (msg.severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
        PHENYL_LOGD(detail::VULKAN_LOGGER, "Vulkan {} message ({}): \"{}\"", type, messageIdName, message);
    } else {
        PHENYL_LOGW(detail::VULKAN_LOGGER, "Vulkan unknown severity {} message: \"{}\"", type, messageIdName, message);
    }
}

std::vector<const char*> VulkanRenderer::GatherValidationLayers () {
    std::vector<const char*> layers{
        "VK_LAYER_KHRONOS_validation"
    };
    FilterValidationLayers(layers);

    for (auto* i : layers) {
        PHENYL_LOGD(detail::VULKAN_LOGGER, "Loading validation layer \"{}\"", i);
    }

    return layers;
}

void VulkanRenderer::FilterValidationLayers (std::vector<const char*>& layers) {
    std::uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> layerProperties(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, layerProperties.data());

    std::unordered_set<const char*> seenLayers;
    for (const auto& properties : layerProperties) {
        PHENYL_LOGD(detail::VULKAN_LOGGER, "Found validation layer \"{}\" (version={}, impl version={}): \"\"",
            properties.layerName, properties.specVersion, properties.implementationVersion, properties.description);
        auto it = std::ranges::find_if(layers, [&] (const auto* x) {
            return std::strcmp(x, properties.layerName) == 0;
        });
        if (it != layers.end()) {
            seenLayers.emplace(*it);
        }
    }

    if (seenLayers.size() == layers.size()) {
        return;
    }

    for (auto* i : layers | std::ranges::views::filter([&] (const char* x) { return !seenLayers.contains(x); })) {
        PHENYL_LOGI(detail::VULKAN_LOGGER, "Dropping unsupported validation layer \"{}\"", i);
    }

    std::erase_if(layers, [&] (const auto* x) {
        return !seenLayers.contains(x);
    });
}


std::vector<const char*> VulkanRenderer::GatherExtensions (const GraphicsProperties& properties, const VulkanViewport& viewport) {
    auto extensions = viewport.getGLFWExtensions();

    // Validation layers
    extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    CheckExtensions(extensions);

    for (auto* i : extensions) {
        PHENYL_LOGD(detail::VULKAN_LOGGER, "Loading extension \"{}\"", i);
    }

    return extensions;
}

void VulkanRenderer::CheckExtensions (const std::vector<const char*>& extensions) {
    std::uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensionProperties(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensionProperties.data());

    std::unordered_set<const char*> seenExtensions;
    for (const auto& properties : extensionProperties) {
        PHENYL_LOGD(detail::VULKAN_LOGGER, "Found extension \"{}\" (version={})", properties.extensionName, properties.specVersion);
        auto it = std::ranges::find_if(extensions, [&] (const auto* x) {
            return std::strcmp(x, properties.extensionName) == 0;
        });
        if (it != extensions.end()) {
            seenExtensions.emplace(*it);
        }
    }

    if (seenExtensions.size() == extensions.size()) {
        return;
    }

    for (auto* i : extensions) {
        if (!seenExtensions.contains(i)) {
            PHENYL_LOGE(detail::VULKAN_LOGGER, "Required extension \"{}\" not supported by system!", i);
        }
    }
    PHENYL_ABORT("Required Vulkan extensions not present!");
}

VkDebugUtilsMessengerCreateInfoEXT VulkanRenderer::getDebugMessengerCreateInfo () {
    return {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
                | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT,
        .pfnUserCallback = DebugMessageCallback,
        .pUserData = this
    };
}

void VulkanRenderer::setupDebugMessenger () {
    auto messengerCreateInfo = getDebugMessengerCreateInfo();

    if (const auto func = LookupVulkanEXT<PFN_vkCreateDebugUtilsMessengerEXT>(instance, "vkCreateDebugUtilsMessengerEXT")) {
        if (auto result = func(instance, &messengerCreateInfo, nullptr, &debugMessenger); result != VK_SUCCESS) {
            PHENYL_LOGE(detail::VULKAN_LOGGER, "Failed to setup debug messenger, error code: {}", result);
        } else {
            PHENYL_LOGD(detail::VULKAN_LOGGER, "Setup Vulkan debug messenger");
        }
    }
}

void VulkanRenderer::destroyDebugMessenger () {
    if (const auto func = LookupVulkanEXT<PFN_vkDestroyDebugUtilsMessengerEXT>(instance, "vkDestroyDebugUtilsMessengerEXT")) {
        func(instance, debugMessenger, nullptr);
    }
}

void VulkanRenderer::createVkInstance (const GraphicsProperties& properties) {
    VkApplicationInfo appInfo{
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "Phenyl Application", // TODO
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "Phenyl Engine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0), // TODO
        .apiVersion = VK_API_VERSION_1_3
    };

    auto validationLayers = GatherValidationLayers();
    auto extensions = GatherExtensions(properties, *viewport);
    auto debugMessengerInfo = getDebugMessengerCreateInfo();

    const VkInstanceCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = &debugMessengerInfo,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = static_cast<std::uint32_t>(validationLayers.size()),
        .ppEnabledLayerNames = validationLayers.data(),
        .enabledExtensionCount = static_cast<std::uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
    };

    if (auto result = vkCreateInstance(&createInfo, nullptr, &instance); result != VK_SUCCESS) {
        PHENYL_ABORT("Failed to create Vulkan instance, error = {}", result);
    }
}

std::unique_ptr<Renderer> phenyl::graphics::MakeVulkanRenderer (const GraphicsProperties& properties) {
    return std::make_unique<VulkanRenderer>(properties, std::make_unique<VulkanViewport>(properties));
}

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugMessageCallback (VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
    auto* messageIdName = pCallbackData->pMessageIdName;
    auto* message = pCallbackData->pMessage;

    std::string_view type;
    if (messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) {
        type = "GENERAL";
    } else if (messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
        type = "VALIDATION";
    } else if (messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
        type = "PERFORMANCE";
    } else if (messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT) {
        type = "DEVICE_ADDRESS_BINDING";
    } else {
        PHENYL_LOGE(phenyl::vulkan::detail::VULKAN_LOGGER, "Received unexpected message type {}", messageType);
        type = "UNEXPECTED";
    }

    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        PHENYL_LOGE(phenyl::vulkan::detail::VULKAN_LOGGER, "Vulkan {} message ({}): {}", type, messageIdName, message);
    } else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        PHENYL_LOGW(phenyl::vulkan::detail::VULKAN_LOGGER, "Vulkan {} message ({}): {}", type, messageIdName, message);
    } else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        PHENYL_LOGD(phenyl::vulkan::detail::VULKAN_LOGGER, "Vulkan {} message ({}): {}", type, messageIdName, message);
    } else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
        PHENYL_TRACE(phenyl::vulkan::detail::VULKAN_LOGGER, "Vulkan {} message ({}): {}", type, messageIdName, message);
    } else {
        PHENYL_LOGW(phenyl::vulkan::detail::VULKAN_LOGGER, "Vulkan unknown severity {} message ({}): \"{}\"", type, messageIdName, message);
    }

    return VK_FALSE;
}