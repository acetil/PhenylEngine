module;

#include "common/assets/assets.h"
#include "common/assets/asset_serializer.h"

export module phenyl.asset;

export namespace phenyl {
    template <typename T>
    using Asset = phenyl::common::Asset<T>;

    template <typename T>
    using AssetManager = phenyl::common::AssetManager<T>;

    using Assets = phenyl::common::Assets;
}

export namespace phenyl::common {
    template<typename T>
    phenyl_AssetSerializer<T> phenyl_serialization_obj (phenyl::common::Asset<T>*);
}