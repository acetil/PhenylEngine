#pragma once

#include <concepts>
#include <functional>
#include <queue>

#include "vulkan/vulkan_headers.h"
#include "logging/logging.h"

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
    private:
        T obj{};
        DestructorFunc destructor;
    public:
        VulkanResource () = default;
        VulkanResource (T obj, DestructorFunc destructor) : obj{std::move(obj)}, destructor{std::move(destructor)} {}

        VulkanResource (const VulkanResource&) = delete;
        VulkanResource (VulkanResource&& other) noexcept : obj{other.obj}, destructor{other.destructor} {
            other.obj = {};
            other.destructor = nullptr;
        }

        VulkanResource& operator= (const VulkanResource&) = delete;
        VulkanResource& operator= (VulkanResource&& other) noexcept {
            if (obj) {
                PHENYL_DASSERT(destructor);
                destructor(std::move(obj));
            }

            obj = other.obj;
            destructor = other.destructor;

            other.obj = {};
            other.destructor = nullptr;

            return *this;
        }

        ~VulkanResource () noexcept {
            if (obj) {
                PHENYL_DASSERT(destructor);
                destructor(std::move(obj));
            }
        }

        explicit operator bool () const noexcept {
            return static_cast<bool>(obj);
        }

        T operator* () const noexcept {
            PHENYL_ASSERT(static_cast<bool>(*this));
            return obj;
        }

        T* operator-> () noexcept {
            PHENYL_ASSERT(static_cast<bool>(*this));
            return &obj;
        }

        const T* operator-> () const noexcept {
            PHENYL_ASSERT(static_cast<bool>(*this));
            return &obj;
        }
    };

    class VulkanDevice;
    struct DeviceProperties;

    class VulkanResources {
    private:
        template <typename T>
        class DestructionQueue {
        private:
            std::function<void(T)> destructor;
            std::queue<std::pair<T, std::uint64_t>> queuedDestructions;

        public:
            explicit DestructionQueue (std::function<void(T)> destructor) : destructor{std::move(destructor)} {}

            ~DestructionQueue() {
                clear();
            }

            void queueDestruction (T obj, std::uint64_t frameNum) {
                queuedDestructions.emplace(std::move(obj), frameNum);
            }

            void onFrame (std::uint64_t frame) {
                while (!queuedDestructions.empty()) {
                    auto [obj, objFrame] = queuedDestructions.front();
                    if (objFrame > frame) {
                        break;
                    }

                    queuedDestructions.pop();
                    destructor(obj);
                }
            }

            void clear () {
                while (!queuedDestructions.empty()) {
                    auto [obj, _] = queuedDestructions.front();
                    queuedDestructions.pop();

                    destructor(obj);
                }
            }
        };

        VulkanDevice& device;
        VmaAllocator allocator;

        DestructionQueue<VulkanBufferInfo> bufferQueue;

        DestructionQueue<VulkanImageInfo> imageQueue;
        DestructionQueue<VkImageView> imageViewQueue;
        DestructionQueue<VkSampler> samplerQueue;

        DestructionQueue<VkDescriptorSetLayout> descriptorSetLayoutQueue;
        DestructionQueue<VkPipelineLayout> pipelineLayoutQueue;
        DestructionQueue<VkPipeline> pipelineQueue;

        DestructionQueue<VkSemaphore> semaphoreQueue;
        DestructionQueue<VkFence> fenceQueue;

        DestructionQueue<VkCommandPool> commandPoolQueue;
        DestructionQueue<VulkanCommandBufferInfo> commandBufferQueue;

        std::uint64_t frameNum = 0;
        std::uint64_t maxFramesInFlight;

        [[nodiscard]] std::uint64_t getDestructionFrame () const noexcept;
    public:
        VulkanResources (VkInstance instance, VulkanDevice& device, std::uint64_t maxFramesInFlight);
        ~VulkanResources ();

        [[nodiscard]] VkDevice getDevice () const noexcept;
        [[nodiscard]] VkQueue getGraphicsQueue () const noexcept;
        [[nodiscard]] const DeviceProperties& getDeviceProperties () const noexcept;

        VulkanResource<VulkanBufferInfo> makeBuffer (const VkBufferCreateInfo& createInfo, const VmaAllocationCreateInfo& allocCreateInfo);

        VulkanResource<VulkanImageInfo> makeImage (const VkImageCreateInfo& createInfo, const VmaAllocationCreateInfo& allocCreateInfo);
        VulkanResource<VkImageView> makeImageView (const VkImageViewCreateInfo& createInfo);
        VulkanResource<VkSampler> makeSampler (const VkSamplerCreateInfo& createInfo);

        VulkanResource<VkDescriptorSetLayout> makeDescriptorSetLayout (const VkDescriptorSetLayoutCreateInfo& layoutInfo);
        VulkanResource<VkPipelineLayout> makePipelineLayout (const VkPipelineLayoutCreateInfo& layoutInfo);
        VulkanResource<VkPipeline> makePipeline (const VkGraphicsPipelineCreateInfo& createInfo);

        VulkanResource<VkCommandPool> makeCommandPool (VkCommandPoolCreateFlags usage = 0);
        VulkanResource<VulkanCommandBufferInfo> makeCommandBuffer (VkCommandPool pool, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

        VulkanResource<VkSemaphore> makeSemaphore ();
        VulkanResource<VkFence> makeFence (const VkFenceCreateInfo& createInfo);

        void onFrame (std::uint64_t frameNum);

    };
}
