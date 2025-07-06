#include "core/assets/assets.h"

#include "core/assets/asset.h"
#include "core/assets/asset_manager.h"

using namespace phenyl::core;

Assets* Assets::INSTANCE = nullptr;

// bool detail::AssetManagerBase::OnUnloadUntyped (meta::TypeIndex typeIndex, std::size_t id) {
//     return true;
// }
//
// std::size_t detail::AssetManagerBase::onVirtualLoadUntyped (meta::TypeIndex, const std::string& virtualPath,
//     std::byte* data) {
//     return 0;
// }

AssetTypeUntyped::AssetTypeUntyped (meta::TypeIndex type) : m_id{0}, m_type{type} {}

AssetTypeUntyped::AssetTypeUntyped (const AssetTypeUntyped& other) : m_id{0}, m_type{other.m_type} {}

AssetTypeUntyped::AssetTypeUntyped (AssetTypeUntyped&& other) noexcept : m_id{0}, m_type{other.m_type} {}

AssetTypeUntyped& AssetTypeUntyped::operator= (const AssetTypeUntyped& other) {
    // Keep old id
    return *this;
}

AssetTypeUntyped& AssetTypeUntyped::operator= (AssetTypeUntyped&& other) noexcept {
    // Keep old id
    return *this;
}

AssetTypeUntyped::~AssetTypeUntyped () {
    if (m_id) {
        Assets::OnUnload2(m_type, m_id);
    }
}
