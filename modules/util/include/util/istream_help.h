#pragma once

#include <istream>

#include "endian.h"

namespace phenyl::util {
    template <std::integral T>
    std::istream& BinaryRead (std::istream& stream, T& val) {
        stream.read(reinterpret_cast<char*>(&val), sizeof(T));
        return stream;
    }

    template <std::integral T>
    std::istream& BinaryReadLittle (std::istream& stream, T& val) {
        T v;
        stream.read(reinterpret_cast<char*>(&v), sizeof(T));
        val = ConvertFromLittle(v);
        return stream;
    }

    template <std::integral T>
    std::istream& BinaryReadBig (std::istream& stream, T& val) {
        T v;
        stream.read(reinterpret_cast<char*>(&v), sizeof(T));
        val = ConvertFromBig(v);
        return stream;
    }
}