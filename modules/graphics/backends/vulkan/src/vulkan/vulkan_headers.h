#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

#include "logging/logging.h"

namespace phenyl::vulkan {
    namespace detail {
        extern phenyl::Logger VULKAN_LOGGER;
    }

    template <typename FunctionType>
    FunctionType LookupVulkanEXT(VkInstance instance, const char* funcName) {
        if (auto func = reinterpret_cast<FunctionType>(vkGetInstanceProcAddr(instance, funcName))) {
            return func;
        } else {
            PHENYL_LOGE(detail::VULKAN_LOGGER, "Failed to lookup Vulkan EXT function {}", funcName);
            return nullptr;
        }
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