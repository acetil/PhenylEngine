#include <cstring>
#include <ranges>

#include "vk_renderer.h"

#include <unordered_set>

#include "vk_array_texture.h"
#include "vk_framebuffer.h"
#include "vk_image_texture.h"
#include "vk_pipeline.h"
#include "vk_storage_buffer.h"
#include "vk_uniform_buffer.h"
#include "core/assets/assets.h"

using namespace phenyl::graphics;
using namespace phenyl::vulkan;

phenyl::Logger phenyl::vulkan::detail::VULKAN_LOGGER{"VULKAN", phenyl::PHENYL_LOGGER};

static constexpr std::size_t MAX_FRAMES_IN_FLIGHT = 2;

namespace phenyl::vulkan {
    class VulkanViewport : public glfw::GLFWViewport {
    private:
        static void VulkanWindowHints () {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

            // TODO
            //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
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
            auto result = glfwCreateWindowSurface(instance, window, nullptr, &surface);
            PHENYL_ASSERT_MSG(result == VK_SUCCESS, "Failed to create window surface!");

            PHENYL_LOGI(detail::VULKAN_LOGGER, "Created Vulkan window surface");
            return surface;
        }
    };
}

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugMessageCallback (VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

VulkanRenderer::VulkanRenderer (const GraphicsProperties& properties, std::unique_ptr<VulkanViewport> vkViewport) : viewport{std::move(vkViewport)} {
    PHENYL_DASSERT(viewport);


    instance = createVkInstance(properties);
    PHENYL_DASSERT(instance);

    setupDebugMessenger();

    surface = viewport->createSurface(instance);

    device = std::make_unique<VulkanDevice>(instance, surface);
    allocator = device->makeVmaAllocator(instance, VK_API_VERSION_1_3);

    swapChain = device->makeSwapChain(surface);
    frameManager = std::make_unique<FrameManager>(*device, MAX_FRAMES_IN_FLIGHT);

    shaderManager = std::make_unique<VulkanShaderManager>(device->device());
    shaderManager->selfRegister();
    PHENYL_LOGI(detail::VULKAN_LOGGER, "Completed renderer setup");
}

VulkanRenderer::~VulkanRenderer () {
    vkDeviceWaitIdle(device->device());
    PHENYL_LOGI(detail::VULKAN_LOGGER, "Destroying Vulkan renderer");
    testPipeline = nullptr;

    shaderManager = nullptr;

    frameManager = nullptr;
    swapChain = nullptr;

    vmaDestroyAllocator(allocator);
    device = nullptr;
    vkDestroySurfaceKHR(instance, surface, nullptr);

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
    if (!frameManager->onNewFrame(*swapChain)) {
        PHENYL_LOGD(detail::VULKAN_LOGGER, "Swapchain recreation requested on frame acquisition");
        recreateSwapChain();
        return;
    }

    const auto& frameImage = frameManager->getImage();
    auto& frameSync = frameManager->getFrameSync();

    auto commandBuffer = frameManager->getCommandPool().getBuffer();
    commandBuffer.doImageTransition(frameImage.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    {
        auto recorder = commandBuffer.beginRendering(frameImage.view, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, swapChain->extent(), VkClearValue{
            .color = {
                .float32 = {0, 0, 0, 1}
            }
        });
        //testPipeline->renderTest(recorder, swapChain->getViewport(), swapChain->getScissor(), 3);

        // TODO
        framebuffer.renderingRecorder = &recorder;
        framebuffer.viewport = swapChain->getViewport();
        framebuffer.scissor = swapChain->getScissor();
        layerRender();
    }

    commandBuffer.doImageTransition(frameImage.image, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    auto recordedBuffer = commandBuffer.record();

    recordedBuffer.submit(device->getGraphicsQueue(), &frameSync.imageAvailable, &frameSync.renderFinished, frameSync.inFlight);

    if (!swapChain->present(device->getGraphicsQueue(), frameSync.renderFinished)) {
        PHENYL_LOGD(detail::VULKAN_LOGGER, "Swapchain recreation requested on frame presentation");
        recreateSwapChain();
    }
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

VulkanBuffer VulkanRenderer::makeBuffer (std::size_t size, bool isIndex) {
    return VulkanBuffer{allocator, static_cast<VkBufferUsageFlags>(isIndex ? VK_BUFFER_USAGE_INDEX_BUFFER_BIT : VK_BUFFER_USAGE_VERTEX_BUFFER_BIT), size};
}

std::unique_ptr<IBuffer> VulkanRenderer::makeRendererBuffer (std::size_t startCapacity, std::size_t elementSize, bool isIndex) {
    return std::make_unique<VulkanStorageBuffer>(*this, startCapacity, isIndex);
}

std::unique_ptr<IUniformBuffer> VulkanRenderer::makeRendererUniformBuffer (bool readable) {
    return std::make_unique<VulkanUniformBuffer>();
}

std::unique_ptr<IImageTexture> VulkanRenderer::makeRendererImageTexture (const TextureProperties& properties) {
    return std::make_unique<VulkanImageTexture>();
}

std::unique_ptr<IImageArrayTexture> VulkanRenderer::makeRendererArrayTexture (const TextureProperties& properties, std::uint32_t width, std::uint32_t height) {
    return std::make_unique<VulkanArrayTexture>();
}

std::unique_ptr<IFrameBuffer> VulkanRenderer::makeRendererFrameBuffer (const FrameBufferProperties& properties, std::uint32_t width, std::uint32_t height) {
    return std::make_unique<VulkanFrameBuffer>();
}

PipelineBuilder VulkanRenderer::buildPipeline () {
    return PipelineBuilder{std::make_unique<VulkanPipelineBuilder>(device->device(), swapChain->format(), &framebuffer)};
}

void VulkanRenderer::loadDefaultShaders () {
    shaderManager->loadDefaultShaders();

    auto builder = VulkanPipelineBuilder{device->device(), swapChain->format(), &framebuffer};
    builder.withShader(core::Assets::Load<Shader>("phenyl/shaders/test"));
    testPipeline = std::unique_ptr<VulkanPipeline>(reinterpret_cast<VulkanPipeline*>(builder.build().release()));
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
    auto layerProperties = Enumerate<VkLayerProperties>(vkEnumerateInstanceLayerProperties);

    std::unordered_set<const char*> seenLayers;
    for (const auto& properties : layerProperties) {
        PHENYL_LOGD(detail::VULKAN_LOGGER, "Found validation layer \"{}\" (version={}, impl version={}): \"\"",
            properties.layerName, VulkanVersion::FromPacked(properties.specVersion), properties.implementationVersion, properties.description);
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
    auto extensionProperties = Enumerate<VkExtensionProperties>(vkEnumerateInstanceExtensionProperties, nullptr);

    std::unordered_set<const char*> seenExtensions;
    for (const auto& properties : extensionProperties) {
        PHENYL_LOGD(detail::VULKAN_LOGGER, "Found extension \"{}\" (version={})", properties.extensionName, VulkanVersion::FromPacked(properties.specVersion));
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
                | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
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

VkInstance VulkanRenderer::createVkInstance (const GraphicsProperties& properties) {
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

    VkInstance instance;
    if (auto result = vkCreateInstance(&createInfo, nullptr, &instance); result != VK_SUCCESS) {
        PHENYL_ABORT("Failed to create Vulkan instance, error = {}", result);
    }

    PHENYL_LOGI(detail::VULKAN_LOGGER, "Created instance for Vulkan version {}", VulkanVersion::FromPacked(VK_API_VERSION_1_3));
    return instance;
}

void VulkanRenderer::recreateSwapChain () {
    PHENYL_LOGI(detail::VULKAN_LOGGER, "Recreating swap chain");
    vkDeviceWaitIdle(device->device());
    swapChain = nullptr;
    swapChain = device->makeSwapChain(surface);
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
        PHENYL_LOGD(phenyl::vulkan::detail::VULKAN_LOGGER, "Vulkan verbose {} message ({}): {}", type, messageIdName, message);
    } else {
        PHENYL_LOGW(phenyl::vulkan::detail::VULKAN_LOGGER, "Vulkan unknown severity {} message ({}): \"{}\"", type, messageIdName, message);
    }

    return VK_FALSE;
}