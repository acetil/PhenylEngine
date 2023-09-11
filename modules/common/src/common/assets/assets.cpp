#include "common/assets/asset.h"
#include "common/assets/assets.h"
#include "common/assets/asset_manager.h"

using namespace common;

Assets* Assets::INSTANCE = nullptr;

bool detail::AssetManagerBase::OnUnloadUntyped (std::size_t typeIndex, std::size_t id) {
    return Assets::UnloadAsset(typeIndex, id);
}

void detail::AssetBase::IncRefCount (std::size_t typeIndex, std::size_t id) {
    Assets::IncrementRefCount(typeIndex, id);
}

void detail::AssetBase::DecRefCount (std::size_t typeIndex, std::size_t id) {
    Assets::DecrementRefCount(typeIndex, id);
}
