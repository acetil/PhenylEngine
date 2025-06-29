#pragma once

#include "init/vk_device.h"
#include "init/vk_swap_chain.h"
#include "vk_command_buffer.h"
#include "vk_descriptors.h"
#include "vk_framebuffer.h"
#include "vk_sync.h"
#include "vulkan_headers.h"

namespace phenyl::vulkan {
struct FrameSync {
    VulkanSemaphore imageAvailable;
    VulkanFence inFlight;
};

class IFrameListener {
public:
    virtual ~IFrameListener () = default;

    virtual void onMaxFramesUpdate (std::size_t maxFrames) = 0;
    virtual void onFrameUpdate (std::size_t currIndex) = 0;
};

class FrameManager {
public:
    FrameManager (VulkanDevice& device, VulkanResources& resources, std::size_t maxInFlight);

    bool onNewFrame (VulkanWindowFrameBuffer& windowFrameBuffer);

    VulkanCommandPool& getCommandPool () {
        PHENYL_DASSERT(flightNum() < m_commandPools.size());
        return m_commandPools[flightNum()];
    }

    FrameSync& getFrameSync () {
        PHENYL_DASSERT(flightNum() < m_syncs.size());
        return m_syncs[flightNum()];
    }

    std::size_t frameNum () const noexcept {
        return m_frameCount;
    }

    std::size_t flightNum () const noexcept {
        return m_frameCount % m_maxInFlight;
    }

    void addFrameListener (IFrameListener* listener);
    void removeFrameListener (IFrameListener* listener);

private:
    std::size_t m_frameCount = static_cast<std::size_t>(-1);
    std::size_t m_maxInFlight;

    std::vector<VulkanCommandPool> m_commandPools;
    std::vector<VulkanDescriptorPool> m_descriptorPools; // TODO: per-pipeline pools?
    std::vector<FrameSync> m_syncs;
    std::unordered_set<IFrameListener*> m_listeners;

    friend class VulkanRenderer;
};

template <typename T>
class FramePool : protected IFrameListener {
public:
    FramePool (
        FrameManager& manager, std::function<T()> factory, std::function<void(T&)> onFrameStart = [] (T&) {}) :
        m_manager{manager},
        m_pool{},
        factory{std::move(factory)},
        onFrameStart{std::move(onFrameStart)} {
        manager.addFrameListener(this);
    }

    ~FramePool () override {
        m_manager.removeFrameListener(this);
    }

    T& get () {
        PHENYL_DASSERT(m_currFrame < m_pool.size());
        return m_pool[m_currFrame];
    }

    const T& get () const {
        PHENYL_DASSERT(m_currFrame < m_pool.size());
        return m_pool[m_currFrame];
    }

protected:
    void onMaxFramesUpdate (std::size_t maxFrames) override {
        for (auto i = m_pool.size(); i < maxFrames; i++) {
            m_pool.emplace_back(factory());
        }

        while (m_pool.size() > maxFrames) {
            m_pool.pop_back();
        }
    }

    void onFrameUpdate (std::size_t currIndex) override {
        PHENYL_DASSERT(currIndex < m_pool.size());
        m_currFrame = currIndex;
        onFrameStart(m_pool[currIndex]);
    }

private:
    FrameManager& m_manager;
    std::vector<T> m_pool;
    std::function<T()> factory;
    std::function<void(T&)> onFrameStart;
    std::size_t m_currFrame = 0;
};
} // namespace phenyl::vulkan
