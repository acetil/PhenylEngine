#include "core/assets/assets.h"

#include "core/assets/asset.h"
#include "core/assets/asset_manager.h"

using namespace phenyl::core;

Assets* Assets::INSTANCE = nullptr;

bool detail::AssetManagerBase::OnUnloadUntyped (meta::TypeIndex typeIndex, std::size_t id) {
    return Assets::UnloadAsset(typeIndex, id);
}

std::size_t detail::AssetManagerBase::onVirtualLoadUntyped (meta::TypeIndex, const std::string& virtualPath,
    std::byte* data) {
    return 0;
}

void detail::AssetBase::IncRefCount (meta::TypeIndex typeIndex, std::size_t id) {
    Assets::IncrementRefCount(typeIndex, id);
}

void detail::AssetBase::DecRefCount (meta::TypeIndex typeIndex, std::size_t id) {
    Assets::DecrementRefCount(typeIndex, id);
}

std::string_view detail::AssetBase::GetPath (meta::TypeIndex typeIndex, std::size_t id) {
    return Assets::GetPath(typeIndex, id);
}
