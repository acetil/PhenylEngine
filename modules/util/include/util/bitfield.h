#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace phenyl::util {
namespace detail {
    static constexpr std::size_t ceil (std::size_t val, std::size_t denom) {
        return val / denom + (val % denom ? 1 : 0);
    }
} // namespace detail

template<std::size_t N>
class Bitfield {
public:
    Bitfield () : m_data{{0}} {}

    Bitfield (const Bitfield<N>& other) {
        std::memcpy(m_data, other.m_data, sizeof(std::size_t) * NumInts);
    }

    Bitfield<N>& operator= (const Bitfield<N>& other) {
        if (this == &other) {
            return *this;
        }
        std::memcpy(m_data, other.m_data, sizeof(std::size_t) * NumInts);

        return *this;
    }

    Bitfield (Bitfield<N>&& other) noexcept {
        std::memcpy(m_data, other.m_data, sizeof(std::size_t) * NumInts);
    }

    Bitfield<N>& operator= (Bitfield<N>&& other) noexcept {
        std::memcpy(m_data, other.m_data, sizeof(std::size_t) * NumInts);

        return *this;
    }

    static Bitfield<N> Mask (std::size_t numBits) {
        PHENYL_DASSERT(numBits <= N);
        Bitfield<N> result;

        std::size_t index = 0;
        while (index * IntSize < numBits) {
            if ((index + 1) * IntSize <= numBits) {
                result.m_data[index] = FullMask;
            } else {
                result.m_data[index] = FullMask & ((1 << (numBits - index * IntSize)) - 1);
            }
            index++;
        }

        return result;
    }

    bool hasBit (std::size_t bit) {
        if (bit >= N) {
            return false;
        }

        return m_data[bit / IntSize] & (1 << (bit % IntSize));
    }

    void putBit (std::size_t bit) {
        PHENYL_DASSERT(bit < N);

        m_data[bit / IntSize] |= 1 << (bit % IntSize);
    }

    void maskBit (std::size_t bit) {
        PHENYL_DASSERT(bit < N);

        m_data[bit / IntSize] &= (FullMask ^ (1 << (bit % IntSize)));
    }

    void clear () {
        std::memset(m_data, 0, sizeof(m_data));
    }

    operator bool () {
        for (std::size_t i = 0; i < NumInts; i++) {
            if (m_data[i]) {
                return true;
            }
        }

        return false;
    }

    Bitfield<N> operator~() {
        Bitfield<N> other{};
        for (std::size_t i = 0; i < FullInts; i++) {
            other.m_data[i] = ~m_data[i];
        }

        if constexpr (NumInts > FullInts) {
            other.m_data[NumInts - 1] = (~m_data[NumInts - 1]) & ((1 << (N % IntSize)) - 1);
        }

        return other;
    }

    Bitfield<N>& operator&= (const Bitfield<N>& other) {
        for (std::size_t i = 0; i < NumInts; i++) {
            m_data[i] &= other.m_data[i];
        }

        return *this;
    }

    Bitfield<N>& operator|= (const Bitfield<N>& other) {
        for (std::size_t i = 0; i < NumInts; i++) {
            m_data[i] |= other.m_data[i];
        }

        return *this;
    }

    Bitfield<N>& operator^= (const Bitfield<N>& other) {
        for (std::size_t i = 0; i < NumInts; i++) {
            m_data[i] ^= other.m_data[i];
        }

        return *this;
    }

    template<std::size_t N2>
    friend bool operator== (const Bitfield<N2>& bitfield1, const Bitfield<N2>& bitfield2);
    template<std::size_t N2>
    friend Bitfield<N2> operator& (const Bitfield<N2>& bitfield1, const Bitfield<N2>& bitfield2);
    template<std::size_t N2>
    friend Bitfield<N2> operator| (const Bitfield<N2>& bitfield1, const Bitfield<N2>& bitfield2);
    template<std::size_t N2>
    friend Bitfield<N2> operator^ (const Bitfield<N2>& bitfield1, const Bitfield<N2>& bitfield2);

private:
    static constexpr std::size_t FullMask = ~((std::size_t) 0);
    static constexpr std::size_t IntSize = sizeof(size_t) * 8;
    static constexpr std::size_t FullInts = N / (IntSize);
    static constexpr std::size_t NumInts = detail::ceil(N, IntSize);

    std::size_t m_data[NumInts];
};

template<std::size_t N>
inline bool operator== (const Bitfield<N>& bitfield1, const Bitfield<N>& bitfield2) {
    for (std::size_t i = 0; i < Bitfield<N>::NumInts; i++) {
        if (bitfield1.m_data[i] != bitfield2.m_data[i]) {
            return false;
        }
    }

    return true;
}

template<std::size_t N>
inline Bitfield<N> operator& (const Bitfield<N>& bitfield1, const Bitfield<N>& bitfield2) {
    Bitfield<N> result{};
    for (std::size_t i = 0; i < Bitfield<N>::NumInts; i++) {
        result.m_data[i] = bitfield1.m_data[i] & bitfield2.m_data[i];
    }

    return result;
}

template<std::size_t N>
inline Bitfield<N> operator| (const Bitfield<N>& bitfield1, const Bitfield<N>& bitfield2) {
    Bitfield<N> result{};
    for (std::size_t i = 0; i < Bitfield<N>::NumInts; i++) {
        result.m_data[i] = bitfield1.m_data[i] | bitfield2.m_data[i];
    }

    return result;
}

template<std::size_t N>
inline Bitfield<N> operator^ (const Bitfield<N>& bitfield1, const Bitfield<N>& bitfield2) {
    Bitfield<N> result{};
    for (std::size_t i = 0; i < Bitfield<N>::NumInts; i++) {
        result.m_data[i] = bitfield1.m_data[i] ^ bitfield2.m_data[i];
    }

    return result;
}
} // namespace phenyl::util
