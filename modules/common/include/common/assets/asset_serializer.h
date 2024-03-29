#pragma once

#include "asset.h"
#include "assets.h"
#include "common/serializer.h"
#include "common/detail/loggers.h"

namespace phenyl::common {
    template <typename T>
    struct phenyl_AssetSerializer {
        static constexpr const char* Name = "Asset";

        static Asset<T> Factory () {
            return Asset<T>{};
        }

        static bool Accept (JsonSerializer& serializer, const Asset<T>& asset) {
            return serializer.visit((std::string)asset.path());
        }

        static bool Accept (const JsonDeserializer& deserializer, Asset<T>& asset) {
            std::string path;
            if (!deserializer.visit(path)) {
                PHENYL_LOGE(detail::ASSET_SERIALIZER_LOGGER, "Failed to parse asset path!");
                return false;
            }

            Asset<T> newAsset = Assets::Load<T>(path);
            if (!newAsset) {
                PHENYL_LOGE(detail::ASSET_SERIALIZER_LOGGER, "Failed to load asset at \"{}\"!", path);
                return false;
            }

            asset = std::move(newAsset);
            return true;
        }
    };

    template <typename T>
    phenyl_AssetSerializer<T> phenyl_serialization_obj(Asset<T>*);
}