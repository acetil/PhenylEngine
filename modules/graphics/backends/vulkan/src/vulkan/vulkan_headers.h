#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

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