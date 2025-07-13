#include "core/assets/assets.h"

#include "core/assets/asset.h"
#include "core/assets/asset_manager.h"

using namespace phenyl::core;

Assets* Assets::INSTANCE = nullptr;

AssetBase::AssetBase (meta::TypeIndex type) : m_id{0}, m_type{type} {}

AssetBase::AssetBase (const AssetBase& other) : m_id{0}, m_type{other.m_type} {}

AssetBase::AssetBase (AssetBase&& other) noexcept : m_id{0}, m_type{other.m_type} {}

AssetBase& AssetBase::operator= (const AssetBase& other) {
    // Keep old id
    return *this;
}

AssetBase& AssetBase::operator= (AssetBase&& other) noexcept {
    // Keep old id
    return *this;
}

AssetBase::~AssetBase () {
    if (m_id) {
        Assets::OnUnload(m_type, m_id);
    }
}
