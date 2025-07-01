#pragma once

#include "logging/logging.h"
#include "vulkan/vulkan_headers.h"

#include <concepts>
#include <functional>
#include <queue>

namespace phenyl::vulkan {
struct VulkanBufferInfo {
    VmaAllocator allocator = nullptr;
    VkBuffer buffer = nullptr;
    VmaAllocation alloc = nullptr;

    explicit operator bool () const noexcept {
        return allocator && buffer && alloc;
    }
};

struct VulkanImageInfo {
    VkImage image = nullptr;
    VmaAllocation alloc = nullptr;

    explicit operator bool () const noexcept {
        return image && alloc;
    }
};

struct VulkanCommandBufferInfo {
    VkCommandPool pool;
    VkCommandBuffer commandBuffer;

    explicit operator bool () const noexcept {
        return pool && commandBuffer;
    }
};

class VulkanResources;

template <std::movable T>
class VulkanResource {
public:
    using DestructorFunc = std::function<void(T&&)>;

    VulkanResource () = default;

    VulkanResource (T obj, DestructorFunc destructor) : m_obj{std::move(obj)}, m_destructor{std::move(destructor)} {}

    VulkanResource (const VulkanResource&) = delete;

    VulkanResource (VulkanResource&& other) noexcept : m_obj{other.m_obj}, m_destructor{other.m_destructor} {
        other.m_obj = {};
        other.m_destructor = nullptr;
    }

    VulkanResource& operator= (const VulkanResource&) = delete;

    VulkanResource& operator= (VulkanResource&& other) noexcept {
        if (m_obj) {
            PHENYL_DASSERT(m_destructor);
            m_destructor(std::move(m_obj));
        }

        m_obj = other.m_obj;
        m_destructor = other.m_destructor;

        other.m_obj = {};
        other.m_destructor = nullptr;

        return *this;
    }

    ~VulkanResource () noexcept {
        if (m_obj) {
            PHENYL_DASSERT(m_destructor);
            m_destructor(std::move(m_obj));
        }
    }

    explicit operator bool () const noexcept {
        return static_cast<bool>(m_obj);
    }

    T operator* () const noexcept {
        PHENYL_ASSERT(static_cast<bool>(*this));
        return m_obj;
    }

    T* operator->() noexcept {
        PHENYL_ASSERT(static_cast<bool>(*this));
        return &m_obj;
    }

    const T* operator->() const noexcept {
        PHENYL_ASSERT(static_cast<bool>(*this));
        return &m_obj;
    }

private:
    T m_obj{};
    DestructorFunc m_destructor;
};

class VulkanDevice;
struct DeviceProperties;

class VulkanResources {
public:
    VulkanResources (VkInstance instance, VulkanDevice& device, std::uint64_t maxFramesInFlight);
    ~VulkanResources ();

    [[nodiscard]] VkDevice getDevice () const noexcept;
    [[nodiscard]] VkQueue getGraphicsQueue () const noexcept;
    [[nodiscard]] const DeviceProperties& getDeviceProperties () const noexcept;

    VulkanResource<VulkanBufferInfo> makeBuffer (const VkBufferCreateInfo& createInfo,
        const VmaAllocationCreateInfo& allocCreateInfo);

    VulkanResource<VulkanImageInfo> makeImage (const VkImageCreateInfo& createInfo,
        const VmaAllocationCreateInfo& allocCreateInfo);
    VulkanResource<VkImageView> makeImageView (const VkImageViewCreateInfo& createInfo);
    VulkanResource<VkSampler> makeSampler (const VkSamplerCreateInfo& createInfo);

    VulkanResource<VkDescriptorSetLayout> makeDescriptorSetLayout (const VkDescriptorSetLayoutCreateInfo& layoutInfo);
    VulkanResource<VkDescriptorPool> makeDescriptorPool (const VkDescriptorPoolCreateInfo& createInfo);
    VulkanResource<VkPipelineLayout> makePipelineLayout (const VkPipelineLayoutCreateInfo& layoutInfo);
    VulkanResource<VkPipeline> makePipeline (const VkGraphicsPipelineCreateInfo& createInfo);

    VulkanResource<VkCommandPool> makeCommandPool (VkCommandPoolCreateFlags usage = 0);
    VulkanResource<VulkanCommandBufferInfo> makeCommandBuffer (VkCommandPool pool,
        VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    VulkanResource<VkSemaphore> makeSemaphore ();
    VulkanResource<VkFence> makeFence (const VkFenceCreateInfo& createInfo);

    void onFrame (std::uint64_t frameNum);

private:
    template <typename T>
    class DestructionQueue {
    public:
        explicit DestructionQueue (std::function<void(T)> destructor) : m_destructor{std::move(destructor)} {}

        ~DestructionQueue () {
            clear();
        }

        void queueDestruction (T obj, std::uint64_t frameNum) {
            m_queuedDestructions.emplace(std::move(obj), frameNum);
        }

        void onFrame (std::uint64_t frame) {
            while (!m_queuedDestructions.empty()) {
                auto [obj, objFrame] = m_queuedDestructions.front();
                if (objFrame > frame) {
                    break;
                }

                m_queuedDestructions.pop();
                m_destructor(obj);
            }
        }

        void clear () {
            while (!m_queuedDestructions.empty()) {
                auto [obj, _] = m_queuedDestructions.front();
                m_queuedDestructions.pop();

                m_destructor(obj);
            }
        }

    private:
        std::function<void(T)> m_destructor;
        std::queue<std::pair<T, std::uint64_t>> m_queuedDestructions;
    };

    VulkanDevice& m_device;
    VmaAllocator m_allocator;

    DestructionQueue<VulkanBufferInfo> m_bufferQueue;

    DestructionQueue<VulkanImageInfo> m_imageQueue;
    DestructionQueue<VkImageView> m_imageViewQueue;
    DestructionQueue<VkSampler> m_samplerQueue;

    DestructionQueue<VkDescriptorSetLayout> m_descriptorSetLayoutQueue;
    DestructionQueue<VkDescriptorPool> m_descriptorPoolQueue;
    DestructionQueue<VkPipelineLayout> m_pipelineLayoutQueue;
    DestructionQueue<VkPipeline> m_pipelineQueue;

    DestructionQueue<VkSemaphore> m_semaphoreQueue;
    DestructionQueue<VkFence> m_fenceQueue;

    DestructionQueue<VkCommandPool> m_commandPoolQueue;
    DestructionQueue<VulkanCommandBufferInfo> m_commandBufferQueue;

    std::uint64_t m_frameNum = 0;
    std::uint64_t m_maxFramesInFlight;

    [[nodiscard]] std::uint64_t getDestructionFrame () const noexcept;
};
} // namespace phenyl::vulkan
