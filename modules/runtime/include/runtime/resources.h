#pragma once

#include <concepts>
#include <tuple>
#include <type_traits>

#include "iresource.h"
#include "resource_manager.h"

namespace phenyl::runtime {
    template <typename T>
    concept ResourceType = std::derived_from<std::remove_cvref_t<T>, IResource>;

    template <ResourceType ...Args>
    struct Resources : std::tuple<Args&...> {
    public:
        explicit Resources (ResourceManager& resManager) : std::tuple<Args&...>{resManager.resource<Args>()...} {}

        template <typename T>
        T& get () const {
            return std::get<T&>(static_cast<const std::tuple<Args&...>&>(*this));
        }
    };

    template <>
    struct Resources<> : std::tuple<> {
        explicit Resources (ResourceManager& resManager) {}
    };

    namespace detail {
        template <typename T>
        struct is_resources : std::false_type {};

        template <ResourceType ...Args>
        struct is_resources<Resources<Args...>> : std::true_type {};

        template <typename T>
        struct is_bundle : std::false_type {};
        template <typename ...Args>
        struct is_bundle<component::Bundle<Args...>> : std::true_type {};

        template <typename T>
        static constexpr bool is_resources_v = is_resources<T>::value;

        template <typename T>
        static constexpr bool is_bundle_v = is_bundle<T>::value;
    }

    template <typename T>
    concept ComponentType = !std::derived_from<std::remove_cvref_t<T>, IResource> && !detail::is_resources_v<std::remove_cvref_t<T>> && !detail::is_bundle_v<std::remove_cvref_t<T>> && !std::same_as<std::remove_cvref_t<T>, component::Entity>;
}

template <typename ...Args>
struct std::tuple_size<phenyl::runtime::Resources<Args...>> : std::tuple_size<std::tuple<Args&...>> {};

template <std::size_t I, typename ...Args>
struct std::tuple_element<I, phenyl::runtime::Resources<Args...>> : std::tuple_element<I, std::tuple<Args&...>>{};