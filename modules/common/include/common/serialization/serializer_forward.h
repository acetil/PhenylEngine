#pragma once

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <string>

namespace phenyl::common {
    class ISerializableBase;
    template <typename T>
    class ISerializable;

    namespace detail {
        template <typename T>
        struct SerializableMarker {};

        ISerializable<bool>& phenyl_GetSerializable (SerializableMarker<bool>);

        ISerializable<std::int8_t>& phenyl_GetSerializable (SerializableMarker<std::int8_t>);
        ISerializable<std::int16_t>& phenyl_GetSerializable (SerializableMarker<std::int16_t>);
        ISerializable<std::int32_t>& phenyl_GetSerializable (SerializableMarker<std::int32_t>);
        ISerializable<std::int64_t>& phenyl_GetSerializable (SerializableMarker<std::int64_t>);

        ISerializable<std::uint8_t>& phenyl_GetSerializable (SerializableMarker<std::uint8_t>);
        ISerializable<std::uint16_t>& phenyl_GetSerializable (SerializableMarker<std::uint16_t>);
        ISerializable<std::uint32_t>& phenyl_GetSerializable (SerializableMarker<std::uint32_t>);
        ISerializable<std::uint64_t>& phenyl_GetSerializable (SerializableMarker<std::uint64_t>);

        ISerializable<float>& phenyl_GetSerializable (SerializableMarker<float>);
        ISerializable<double>& phenyl_GetSerializable (SerializableMarker<double>);

        ISerializable<std::string>& phenyl_GetSerializable (SerializableMarker<std::string>);

        template <typename T>
        ISerializable<T>& GetSerializable () {
            using ::phenyl::common::detail::phenyl_GetSerializable;
            return phenyl_GetSerializable(SerializableMarker<T>{});
        }
    }

    template <typename T>
    concept SerializableType = requires
    {
        { detail::GetSerializable<T>() } -> std::same_as<ISerializable<T>&>;
    };
}

#define PHENYL_DECLARE_SERIALIZABLE(T) ::phenyl::common::ISerializable<T>& phenyl_GetSerializable (::phenyl::common::detail::SerializableMarker<T>);
#define PHENYL_SERIALIZABLE_INTRUSIVE(T) friend ::phenyl::common::ISerializable<T>& phenyl_GetSerializable (::phenyl::common::detail::SerializableMarker<T>);