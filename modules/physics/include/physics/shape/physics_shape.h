#pragma once

#include <string>
#include <cassert>

#include "util/meta.h"

namespace physics {
    enum class PrimitiveShape {
        NONE,
        RECT_2D,
        CIRCLE_2D,
        CUSTOM
    };

    template <typename T>
    concept ShapeDataType = requires (T t) {
        T::Type;
        requires T::Type > 0;
        T{std::declval<void*>()};
    };

    template <typename T>
    class ShapeView;

    class ShapeData {
    private:
        std::size_t shapeTypeIndex;
        void* data;
    public:
        ShapeData (std::size_t shapeTypeIndex, void* data) : shapeTypeIndex{shapeTypeIndex}, data{data} {}

        template <typename T>
        ShapeView<T> as () {
            assert(meta::type_index<T>() == shapeTypeIndex);

            return ShapeView<T>{data};
        }
    };

    template <typename T>
    class ShapeRequest;
}