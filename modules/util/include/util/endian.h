#pragma once

#include <bit>
#include <concepts>

namespace phenyl::util {
static constexpr bool BigEndian = std::endian::native == std::endian::big;
static constexpr bool LittleEndian = std::endian::native == std::endian::little;

template<std::integral T>
constexpr T ByteSwap (T val) {
    auto* data = reinterpret_cast<std::byte*>(&val);
    T newVal;
    auto* newValData = reinterpret_cast<std::byte*>(&newVal);
    for (auto i = 0; i < sizeof(T); i++) {
        newValData[i] = data[sizeof(T) - i - 1];
    }

    return newVal;
}

template<std::integral T>
constexpr std::enable_if_t<LittleEndian, T> ConvertToLittle (T val) {
    return val;
}

template<std::integral T, typename = std::enable_if_t<LittleEndian, void>>
constexpr std::enable_if_t<LittleEndian, T> ConvertFromLittle (T val) {
    return val;
}

template<std::integral T, typename = std::enable_if_t<LittleEndian, void>>
constexpr std::enable_if_t<LittleEndian, T> ConvertToBig (T val) {
    return ByteSwap(val);
}

template<std::integral T, typename = std::enable_if_t<LittleEndian, void>>
constexpr std::enable_if_t<LittleEndian, T> ConvertFromBig (T val) {
    return ByteSwap(val);
}

template<std::integral T>
constexpr std::enable_if_t<BigEndian, T> ConvertToLittle (T val) {
    return ByteSwap(val);
}

template<std::integral T>
constexpr std::enable_if_t<BigEndian, T> ConvertFromLittle (T val) requires (BigEndian)
{
    return ByteSwap(val);
}

template<std::integral T>
constexpr std::enable_if_t<BigEndian, T> ConvertToBig (T val) {
    return val;
}

template<std::integral T>
constexpr std::enable_if_t<BigEndian, T> ConvertFromBig (T val) {
    return val;
}
} // namespace phenyl::util
