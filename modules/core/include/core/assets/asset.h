#pragma once

#include "core/serialization/serializer.h"
#include "forward.h"
#include "logging/logging.h"
#include "util/type_index.h"

#include <cassert>
#include <cstddef>
#include <string>

namespace phenyl::core {
template <typename T>
class IAssetType;

namespace detail {
    template <typename T>
    class AssetCache;
} // namespace detail

class AssetTypeUntyped {
public:
    [[nodiscard]] std::size_t assetId () const noexcept {
        return m_id;
    }

protected:
    explicit AssetTypeUntyped (meta::TypeIndex type);
    AssetTypeUntyped (const AssetTypeUntyped& other);
    AssetTypeUntyped (AssetTypeUntyped&& other) noexcept;

    AssetTypeUntyped& operator= (const AssetTypeUntyped& other);
    AssetTypeUntyped& operator= (AssetTypeUntyped&& other) noexcept;
    virtual ~AssetTypeUntyped ();

private:
    std::size_t m_id;
    meta::TypeIndex m_type;

    friend class Assets;
    template <typename T>
    friend class detail::AssetCache;
};

template <typename T>
class AssetType : public AssetTypeUntyped {
public:
    AssetType () : AssetTypeUntyped{meta::TypeIndex::Get<T>()} {}
};

template <typename T> concept IsAssetType = std::derived_from<T, AssetType<T>>;

template <IsAssetType T>
ISerializable<std::shared_ptr<T>>& phenyl_GetSerializable(detail::SerializableMarker<std::shared_ptr<T>>);
} // namespace phenyl::core
