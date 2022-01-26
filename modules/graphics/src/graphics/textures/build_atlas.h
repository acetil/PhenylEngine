#pragma once

#include <algorithm>
#include <iterator>
#include <vector>
#include <utility>

namespace graphics {
    template<class T>
    struct AtlasObject {
        int xOff;
        int yOff;
        float uOff;
        float vOff;
        float uSize;
        float vSize;
        T key;
        AtlasObject (int _xOff, int _yOff, float _uOff, float _vOff, float _uSize, float _vSize, T _key) : xOff(_xOff), yOff(_yOff),
                uOff(_uOff), vOff(_vOff), uSize(_uSize), vSize(_vSize), key(_key) {}
    };
    namespace atlas_internal {
        struct InternalAtlasImage {
            int xSize = 0;
            int ySize = 0;
            int index = 0;

            InternalAtlasImage () = default;

            InternalAtlasImage (int _xSize, int _ySize, int _index) : xSize(_xSize), ySize(_ySize), index(_index) {}
        };
        std::pair<std::vector<AtlasObject<int>>, int> buildAtlasInternal (std::vector<InternalAtlasImage>& imgs, int padding);
    }



    template <std::bidirectional_iterator InputIt, class OutputIt>
    int buildAtlas (InputIt start, InputIt end, OutputIt output, int padding) {
        //static_assert(!std::is_same_v<int, decltype((*start).getKey())>, "Output iterator is incorrect!");
        //std::copy()
        using KeyType = decltype((*start).getKey());
        //static_assert(std::is_same_v<KeyType, decltype(start->getKey())>, "Key types do not match!");
        std::vector<KeyType> keys;
        std::vector<atlas_internal::InternalAtlasImage> imgs;
        for (std::bidirectional_iterator auto i = start; i < end; i++) {
            int n = keys.size();
            keys.emplace_back((*i).getKey());
            imgs.emplace_back((*i).getXSize(), (*i).getYSize(), n);
        }
        auto p = atlas_internal::buildAtlasInternal(imgs, padding);
        for (auto i : p.first) {
            *(output) = AtlasObject<KeyType>(i.xOff, i.yOff, i.uOff, i.vOff, i.uSize, i.vSize, keys[i.key]);
            output++;
        }
        return p.second;
    }
}
