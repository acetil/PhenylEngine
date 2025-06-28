#define GLFW_INCLUDE_VULKAN
#include "vk_renderer.h"

#include "core/assets/assets.h"
#include "glfw/glfw_viewport.h"
#include "vk_array_texture.h"
#include "vk_framebuffer.h"
#include "vk_image_texture.h"
#include "vk_pipeline.h"
#include "vk_storage_buffer.h"
#include "vk_uniform_buffer.h"

#include <cstring>
#include <ranges>
#include <unordered_set>

using namespace phenyl::graphics;
using namespace phenyl::vulkan;

phenyl::Logger phenyl::vulkan::detail::VULKAN_LOGGER{"VULKAN", phenyl::PHENYL_LOGGER};

static constexpr std::size_t MAX_FRAMES_IN_FLIGHT = 2;

namespace phenyl::vulkan {
class VulkanViewport : public glfw::GLFWViewport {
private:
    static void VulkanWindowHints () {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    }

public:
    explicit VulkanViewport (const GraphicsProperties& properties) : GLFWViewport{properties, VulkanWindowHints} {}

    [[nodiscard]] std::vector<const char*> getGLFWExtensions () const noexcept {
        std::uint32_t extensionNum = 0;
        auto** extensions = glfwGetRequiredInstanceExtensions(&extensionNum);

        return {extensions, extensions + extensionNum};
    }

    VkSurfaceKHR createSurface (VkInstance instance) {
        VkSurfaceKHR surface;
        auto result = glfwCreateWindowSurface(instance, m_window, nullptr, &surface);
        PHENYL_ASSERT_MSG(result == VK_SUCCESS, "Failed to create window surface!");

        PHENYL_LOGI(detail::VULKAN_LOGGER, "Created Vulkan window surface");
        return surface;
    }
};
} // namespace phenyl::vulkan

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugMessageCallback (VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData);

VulkanRenderer::VulkanRenderer (const GraphicsProperties& properties, std::unique_ptr<VulkanViewport> vkViewport) :
    m_viewport{std::move(vkViewport)} {
    PHENYL_DASSERT(m_viewport);

    m_instance = createVkInstance(properties);
    PHENYL_DASSERT(m_instance);

    setupDebugMessenger();

    m_surface = m_viewport->createSurface(m_instance);

    m_device = std::make_unique<VulkanDevice>(m_instance, m_surface);
    m_resources = std::make_unique<VulkanResources>(m_instance, *m_device, MAX_FRAMES_IN_FLIGHT);

    m_swapChain = m_device->makeSwapChain(*m_resources, m_surface);

    m_frameManager = std::make_unique<FrameManager>(*m_device, *m_resources, MAX_FRAMES_IN_FLIGHT);
    m_transferManager = std::make_unique<TransferManager>(*m_resources);

    m_windowFrameBuffer = std::make_unique<VulkanWindowFrameBuffer>(*m_resources, *m_transferManager, m_fbLayoutManager,
        m_swapChain->format());
    m_windowFrameBuffer->onSwapChainRecreate(m_swapChain.get());

    m_shaderManager = std::make_unique<VulkanShaderManager>(m_device->device());
    m_shaderManager->selfRegister();
    PHENYL_LOGI(detail::VULKAN_LOGGER, "Completed renderer setup");
}

VulkanRenderer::~VulkanRenderer () {
    vkDeviceWaitIdle(m_device->device());
    PHENYL_LOGI(detail::VULKAN_LOGGER, "Destroying Vulkan renderer");

    m_shaderManager = nullptr;

    m_windowFrameBuffer = nullptr;

    m_transferManager = nullptr;
    m_frameManager = nullptr;
    m_swapChain = nullptr;

    m_resources = nullptr;
    m_device = nullptr;
    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

    destroyDebugMessenger();
    vkDestroyInstance(m_instance, nullptr);

    m_viewport = nullptr;
}

double VulkanRenderer::getCurrentTime () {
    return m_viewport->getTime();
}

void VulkanRenderer::clearWindow () {}

void VulkanRenderer::render () {
    if (!m_frameManager->onNewFrame(*m_windowFrameBuffer)) {
        PHENYL_LOGD(detail::VULKAN_LOGGER, "Swapchain recreation requested on frame acquisition");
        recreateSwapChain();
        return;
    }

    auto& frameSync = m_frameManager->getFrameSync();
    auto commandBuffer = m_frameManager->getCommandPool().getBuffer();

    m_framebuffer.renderingRecorder = &commandBuffer;
    m_framebuffer.descriptorPool = &m_frameManager->getDescriptorPool();

    layerRender();

    m_windowFrameBuffer->doPresentTransition(commandBuffer);

    commandBuffer.submit(&frameSync.imageAvailable, m_swapChain->imageSemaphore(), &frameSync.inFlight);

    if (!m_swapChain->present(m_device->graphicsQueue())) {
        PHENYL_LOGD(detail::VULKAN_LOGGER, "Swapchain recreation requested on frame presentation");
        recreateSwapChain();
    }
}

void VulkanRenderer::finishRender () {}

Viewport& VulkanRenderer::getViewport () {
    return *m_viewport;
}

const Viewport& VulkanRenderer::getViewport () const {
    return *m_viewport;
}

std::string_view VulkanRenderer::getName () const noexcept {
    return "VulkanRenderer";
}

std::unique_ptr<IBuffer> VulkanRenderer::makeRendererBuffer (std::size_t startCapacity, std::size_t elementSize,
    graphics::BufferStorageHint storageHint, bool isIndex) {
    switch (storageHint) {
    case BufferStorageHint::STATIC:
        return std::make_unique<VulkanStaticStorageBuffer>(*m_resources, *m_transferManager, isIndex);
    case BufferStorageHint::DYNAMIC:
        return std::make_unique<VulkanStorageBuffer>(*m_resources, startCapacity, isIndex);
    }

    PHENYL_ABORT("Unexpected storage hint: {}", static_cast<std::uint32_t>(storageHint));
}

std::unique_ptr<IUniformBuffer> VulkanRenderer::makeRendererUniformBuffer (bool readable) {
    return std::make_unique<VulkanUniformBuffer>(*m_resources);
}

std::unique_ptr<IImageTexture> VulkanRenderer::makeRendererImageTexture (const TextureProperties& properties) {
    return std::make_unique<VulkanImageTexture>(*m_resources, *m_transferManager, properties);
}

std::unique_ptr<IImageArrayTexture> VulkanRenderer::makeRendererArrayTexture (const TextureProperties& properties,
    std::uint32_t width, std::uint32_t height) {
    return std::make_unique<VulkanArrayTexture>(*m_resources, *m_transferManager, properties, width, height);
}

std::unique_ptr<IFrameBuffer> VulkanRenderer::makeRendererFrameBuffer (const FrameBufferProperties& properties,
    std::uint32_t width, std::uint32_t height) {
    return std::make_unique<VulkanFrameBuffer>(*m_resources, m_fbLayoutManager, properties, width, height);
}

PipelineBuilder VulkanRenderer::buildPipeline () {
    return PipelineBuilder{std::make_unique<VulkanPipelineBuilder>(*m_resources, m_swapChain->format(), &m_framebuffer,
        m_windowFrameBuffer.get())};
}

void VulkanRenderer::loadDefaultShaders () {
    m_shaderManager->loadDefaultShaders();
}

std::vector<const char*> VulkanRenderer::GatherValidationLayers () {
    std::vector<const char*> layers{"VK_LAYER_KHRONOS_validation"};
    FilterValidationLayers(layers);

    for (auto* i : layers) {
        PHENYL_LOGD(detail::VULKAN_LOGGER, "Loading validation layer \"{}\"", i);
    }

    return layers;
}

void VulkanRenderer::FilterValidationLayers (std::vector<const char*>& layers) {
    auto layerProperties = Enumerate<VkLayerProperties>(vkEnumerateInstanceLayerProperties);

    std::unordered_set<const char*> seenLayers;
    for (const auto& properties : layerProperties) {
        PHENYL_LOGD(detail::VULKAN_LOGGER, "Found validation layer \"{}\" (version={}, impl version={}): \"\"",
            properties.layerName, VulkanVersion::FromPacked(properties.specVersion), properties.implementationVersion,
            properties.description);
        auto it =
            std::ranges::find_if(layers, [&] (const auto* x) { return std::strcmp(x, properties.layerName) == 0; });
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

    std::erase_if(layers, [&] (const auto* x) { return !seenLayers.contains(x); });
}

std::vector<const char*> VulkanRenderer::GatherExtensions (const GraphicsProperties& properties,
    const VulkanViewport& viewport) {
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
    auto extensionProperties = Enumerate<VkExtensionProperties>(vkEnumerateInstanceExtensionProperties, nullptr);

    std::unordered_set<const char*> seenExtensions;
    for (const auto& properties : extensionProperties) {
        PHENYL_LOGD(detail::VULKAN_LOGGER, "Found extension \"{}\" (version={})", properties.extensionName,
            VulkanVersion::FromPacked(properties.specVersion));
        auto it = std::ranges::find_if(extensions,
            [&] (const auto* x) { return std::strcmp(x, properties.extensionName) == 0; });
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
      .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
          VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
          VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
      .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
          VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
      .pfnUserCallback = DebugMessageCallback,
      .pUserData = this,
    };
}

void VulkanRenderer::setupDebugMessenger () {
    auto messengerCreateInfo = getDebugMessengerCreateInfo();

    if (const auto func =
            LookupVulkanEXT<PFN_vkCreateDebugUtilsMessengerEXT>(m_instance, "vkCreateDebugUtilsMessengerEXT")) {
        if (auto result = func(m_instance, &messengerCreateInfo, nullptr, &m_debugMessenger); result != VK_SUCCESS) {
            PHENYL_LOGE(detail::VULKAN_LOGGER, "Failed to setup debug messenger, error code: {}", result);
        } else {
            PHENYL_LOGD(detail::VULKAN_LOGGER, "Setup Vulkan debug messenger");
        }
    }
}

void VulkanRenderer::destroyDebugMessenger () {
    if (const auto func =
            LookupVulkanEXT<PFN_vkDestroyDebugUtilsMessengerEXT>(m_instance, "vkDestroyDebugUtilsMessengerEXT")) {
        func(m_instance, m_debugMessenger, nullptr);
    }
}

VkInstance VulkanRenderer::createVkInstance (const GraphicsProperties& properties) {
    VkApplicationInfo appInfo{
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pApplicationName = "Phenyl Application", // TODO
      .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
      .pEngineName = "Phenyl Engine",
      .engineVersion = VK_MAKE_VERSION(1, 0, 0), // TODO
      .apiVersion = VK_API_VERSION_1_3,
    };

    auto validationLayers = GatherValidationLayers();
    auto extensions = GatherExtensions(properties, *m_viewport);
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

    VkInstance instance;
    if (auto result = vkCreateInstance(&createInfo, nullptr, &instance); result != VK_SUCCESS) {
        PHENYL_ABORT("Failed to create Vulkan instance, error = {}", result);
    }

    PHENYL_LOGI(detail::VULKAN_LOGGER, "Created instance for Vulkan version {}",
        VulkanVersion::FromPacked(VK_API_VERSION_1_3));
    return instance;
}

void VulkanRenderer::recreateSwapChain () {
    PHENYL_LOGI(detail::VULKAN_LOGGER, "Recreating swap chain");
    vkDeviceWaitIdle(m_device->device());
    m_swapChain = nullptr;
    m_swapChain = m_device->makeSwapChain(*m_resources, m_surface);

    m_windowFrameBuffer->onSwapChainRecreate(m_swapChain.get());
}

std::unique_ptr<Renderer> phenyl::graphics::MakeVulkanRenderer (const GraphicsProperties& properties) {
    return std::make_unique<VulkanRenderer>(properties, std::make_unique<VulkanViewport>(properties));
}

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugMessageCallback (VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {
    auto* messageIdName = pCallbackData->pMessageIdName ? pCallbackData->pMessageIdName : "(null)";
    auto* message = pCallbackData->pMessage ? pCallbackData->pMessage : "";

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
        PHENYL_LOGD(phenyl::vulkan::detail::VULKAN_LOGGER, "Vulkan verbose {} message ({}): {}", type, messageIdName,
            message);
    } else {
        PHENYL_LOGW(phenyl::vulkan::detail::VULKAN_LOGGER, "Vulkan unknown severity {} message ({}): \"{}\"", type,
            messageIdName, message);
    }

    return VK_FALSE;
}
