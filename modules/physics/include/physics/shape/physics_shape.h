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

    // TODO: refcounting
    class ShapeData {
    private:
        std::size_t shapeType;
        void* data;
        void errExit (const std::string& msg);
    public:
        ShapeData (std::size_t shapeType, void* data) {}

        template <ShapeDataType T>
        T as () {
            if (T::Type != shapeType) {
                errExit("Incorrect shape type!");
            }

            return T{data};
        }
    };

    template <typename T>
    class ShapeView;

    class ShapeDataNew {
    private:
        std::size_t shapeTypeIndex;
        void* data;
    public:
        ShapeDataNew (std::size_t shapeTypeIndex, void* data) : shapeTypeIndex{shapeTypeIndex}, data{data} {}

        template <typename T>
        ShapeView<T> as () {
            assert(meta::type_index<T>() == shapeTypeIndex);

            return ShapeView<T>{data};
        }
    };

    template <typename T>
    class ShapeRequest;
}