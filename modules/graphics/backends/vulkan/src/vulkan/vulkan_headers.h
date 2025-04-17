#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

#include <vk_mem_alloc.h>

#include "logging/logging.h"

namespace phenyl::vulkan {
    namespace detail {
        extern phenyl::Logger VULKAN_LOGGER;
    }

    struct VulkanVersion {
        std::uint32_t variant = 0;
        std::uint32_t major = 1;
        std::uint32_t minor = 3;
        std::uint32_t patch = 0;

        static VulkanVersion FromPacked (std::uint32_t packed) {
            return VulkanVersion{
                .variant = VK_API_VERSION_VARIANT(packed),
                .major = VK_API_VERSION_MAJOR(packed),
                .minor = VK_API_VERSION_MINOR(packed),
                .patch = VK_API_VERSION_PATCH(packed)
            };
        }

        [[nodiscard]] std::uint32_t packed () const noexcept {
            return VK_MAKE_API_VERSION(variant, major, minor, patch);
        }
    };

    template <typename FunctionType>
    FunctionType LookupVulkanEXT(VkInstance instance, const char* funcName) {
        if (auto func = reinterpret_cast<FunctionType>(vkGetInstanceProcAddr(instance, funcName))) {
            return func;
        } else {
            PHENYL_LOGE(detail::VULKAN_LOGGER, "Failed to lookup Vulkan EXT function {}", funcName);
            return nullptr;
        }
    }

    template <typename T, typename F, typename ...Args>
    std::vector<T> Enumerate (F enumerator, Args... args) {
        std::uint32_t objectCount{};
        enumerator(args..., &objectCount, nullptr);

        std::vector<T> objects(objectCount);
        enumerator(args..., &objectCount, objects.data());

        return objects;
    }
}

template <>
struct std::formatter<VkResult, char> {
    template <class ParseContext>
    constexpr ParseContext::iterator parse (ParseContext& ctx) {
        return ctx.begin();
    }

    template <class FmtContext>
    FmtContext::iterator format (const VkResult& result, FmtContext& ctx) const {
        return std::format_to(ctx.out(), "{}", string_VkResult(result));
    }
};

template<>
struct std::formatter<VkImageLayout, char> {
    template <class ParseContext>
constexpr ParseContext::iterator parse (ParseContext& ctx) {
        return ctx.begin();
    }

    template <class FmtContext>
    FmtContext::iterator format (const VkImageLayout& layout, FmtContext& ctx) const {
        return std::format_to(ctx.out(), "{}", string_VkImageLayout(layout));
    }
};

template <>
struct std::formatter<phenyl::vulkan::VulkanVersion, char> {
    template <class ParseContext>
    constexpr ParseContext::iterator parse (ParseContext& ctx) {
        return ctx.begin();
    }

    template <class FmtContext>
    FmtContext::iterator format (const phenyl::vulkan::VulkanVersion& version, FmtContext& ctx) const {
        if (version.variant == 0) {
            return std::format_to(ctx.out(), "{}.{}.{}", version.major, version.minor, version.patch);
        } else {
            return std::format_to(ctx.out(), "{}.{}.{}-Variant {}", version.major, version.minor, version.patch, version.variant);
        }
    }
};

inline bool operator== (const VkViewport& lhs, const VkViewport& rhs) noexcept {
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.width == rhs.width && lhs.height == rhs.height
        && lhs.minDepth == rhs.minDepth && lhs.maxDepth == rhs.maxDepth;
}

inline bool operator== (const VkExtent2D& lhs, const VkExtent2D& rhs) noexcept {
    return lhs.width == rhs.width && lhs.height == rhs.height;
}

inline bool operator== (const VkOffset2D& lhs, const VkOffset2D& rhs) noexcept {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

inline bool operator== (const VkRect2D& lhs, const VkRect2D& rhs) noexcept {
    return lhs.extent == rhs.extent && lhs.offset == rhs.offset;
}