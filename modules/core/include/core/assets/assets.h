#pragma once

#include "asset.h"
#include "asset_manager.h"
#include "core/detail/loggers.h"
#include "forward.h"
#include "util/fl_vector.h"
#include "util/hash.h"
#include "util/type_index.h"

#include <fstream>
#include <string>
#include <utility>

namespace phenyl::core {
namespace detail {
    class IAssetCache {
    public:
        virtual ~IAssetCache () = default;

        virtual void remove (std::size_t assetId) = 0;
    };

    template <typename T>
    class AssetCache : public IAssetCache {
    public:
        AssetCache (AssetManager<T>& manager) : m_manager{manager} {}

        std::shared_ptr<T> lookup (std::string_view path) {
            auto pathIt = m_pathMap.find(path);
            if (pathIt == m_pathMap.end()) {
                return nullptr;
            }

            const CacheItem& item = m_cache.at(pathIt->second);
            if (auto ptr = item.obj.lock(); ptr) {
                return ptr;
            } else {
                m_cache.erase(pathIt->second);
                m_pathMap.erase(pathIt);
                return nullptr;
            }
        }

        std::shared_ptr<T> load (std::string_view path, AssetLoadContext& ctx) {
            PHENYL_DASSERT(!m_pathMap.contains(path));

            auto ptr = m_manager.load(ctx);
            if (!ptr) {
                return nullptr;
            }

            completeLoad(path, ptr);
            return ptr;
        }

        void virtualLoad (std::string_view path, const std::shared_ptr<T>& ptr) {
            PHENYL_ASSERT_MSG(!m_pathMap.contains(path), "Attempted to virtually load asset {} twice!", path);
            completeLoad(path, ptr);
        }

        void remove (std::size_t assetId) override {
            auto it = m_cache.find(assetId);
            if (it == m_cache.end()) {
                return;
            }

            m_pathMap.erase(it->second.path);
            m_cache.erase(it);
        }

        AssetManager<T>& manager () {
            return m_manager;
        }

    private:
        struct CacheItem {
            std::weak_ptr<T> obj;
            std::string path;
        };

        AssetManager<T>& m_manager;

        util::HashMap<std::string, std::size_t> m_pathMap;
        std::unordered_map<std::size_t, CacheItem> m_cache;
        std::size_t m_nextId = 1;

        void completeLoad (std::string_view path, const std::shared_ptr<T>& ptr) {
            PHENYL_DASSERT(ptr);
            auto id = m_nextId++;
            auto& obj = static_cast<AssetTypeUntyped&>(*ptr);
            obj.m_id = id;

            m_pathMap.emplace(path, id);
            m_cache.emplace(id,
                CacheItem{
                  .obj = ptr,
                  .path = std::string{path},
                });
        }
    };
} // namespace detail

class Assets {
public:
    template <IsAssetType T>
    static std::shared_ptr<T> Load (std::string_view path) {
        return GetInstance()->load<T>(path);
    }

    template <IsAssetType T>
    static void LoadVirtual (std::string_view path, const std::shared_ptr<T>& obj) {
        GetInstance()->virtualLoad(path, obj);
    }

    template <IsAssetType T>
    static void AddManager (AssetManager<T>* manager) {
        GetInstance()->addManager(manager);
    }

    template <IsAssetType T>
    static void RemoveManager (AssetManager<T>* manager) {
        GetInstance()->removeManager(manager);
    }

private:
    static Assets* INSTANCE;

    static Assets* GetInstance () {
        if (!INSTANCE) {
            [[unlikely]] INSTANCE = new Assets();
        }

        return INSTANCE;
    }

    std::unordered_map<meta::TypeIndex, std::unique_ptr<detail::IAssetCache>> m_caches2;

    void unloadAsset (meta::TypeIndex type, std::size_t id) {
        PHENYL_DASSERT(m_caches2.contains(type));
        PHENYL_DASSERT(id);
        return m_caches2.at(type)->remove(id);
    }

    template <IsAssetType T>
    std::shared_ptr<T> load (std::string_view path) {
        auto type = meta::TypeIndex::Get<T>();
        auto cacheIt = m_caches2.find(type);
        if (cacheIt == m_caches2.end()) {
            PHENYL_LOGE(detail::ASSETS_LOGGER, "Attempted to load asset of unknown type!");
            return nullptr;
        }
        detail::AssetCache<T>& cache = static_cast<detail::AssetCache<T>&>(*cacheIt->second);

        if (auto ptr = cache.lookup(path)) {
            return ptr;
        }

        // std::ifstream file;
        // if (cache.manager().isBinary()) {
        //     file = std::ifstream{std::string{path} + cache.manager().getFileType(), std::ios::binary};
        // } else {
        //     file = std::ifstream{std::string{path} + cache.manager().getFileType()};
        // }
        // if (!file) {
        //     PHENYL_LOGE(detail::ASSETS_LOGGER, "Failed to open file at \"{}{}\"!", path,
        //     cache.manager().getFileType()); return nullptr;
        // }

        AssetLoadContext ctx{std::string{path}};
        auto ptr = cache.load(path, ctx);
        PHENYL_LOGE_IF(!ptr, detail::ASSETS_LOGGER, "Failed to load asset at \"{}\"!", path);
        return ptr;
    }

    template <IsAssetType T>
    void virtualLoad (std::string_view path, const std::shared_ptr<T>& obj) {
        auto type = meta::TypeIndex::Get<T>();
        auto cacheIt = m_caches2.find(type);
        if (cacheIt == m_caches2.end()) {
            PHENYL_LOGE(detail::ASSETS_LOGGER, "Attempted to virtually load asset of unknown type!");
            return;
        }
        auto& cache = static_cast<detail::AssetCache<T>&>(*cacheIt->second);

        cache.virtualLoad(path, obj);
    }

    template <IsAssetType T>
    void addManager (AssetManager<T>* manager) {
        if (m_caches2.contains(meta::TypeIndex::Get<T>())) {
            PHENYL_LOGE(detail::ASSETS_LOGGER, "Attempted to add asset manager that has already been added!");
            return;
        }

        m_caches2.emplace(meta::TypeIndex::Get<T>(), std::make_unique<detail::AssetCache<T>>(*manager));
    }

    template <IsAssetType T>
    void removeManager (AssetManager<T>* manager) {
        if (!m_caches2.contains(meta::TypeIndex::Get<T>())) {
            PHENYL_LOGE(detail::ASSETS_LOGGER, "Attempted to remove asset manager that does not exist!");
            return;
        }

        m_caches2.erase(meta::TypeIndex::Get<T>());
    }

    static void OnUnload2 (meta::TypeIndex type, std::size_t id) {
        GetInstance()->unloadAsset(type, id);
    }

    friend AssetTypeUntyped;
};

namespace detail {
    template <IsAssetType T>
    class AssetSerializable2 : public ISerializable<std::shared_ptr<T>> {
    public:
        std::string_view name () const noexcept override {
            return "Asset";
        }

        void serialize (ISerializer& serializer, const std::shared_ptr<T>& obj) override {
            serializer.serialize(obj->assetId());
        }

        void deserialize (IDeserializer& deserializer, std::shared_ptr<T>& obj) override {
            deserializer.deserializeString(*this, obj);
        }

        void deserializeString (std::shared_ptr<T>& obj, std::string_view string) override {
            obj = Assets::Load<T>(string); // TODO
            if (!obj) {
                throw DeserializeException(std::format("Failed to load asset at \"{}\"", string));
            }
        }
    };
} // namespace detail

template <IsAssetType T>
ISerializable<std::shared_ptr<T>>& phenyl_GetSerializable (detail::SerializableMarker<std::shared_ptr<T>>) {
    static detail::AssetSerializable2<T> serializable;
    return serializable;
}
} // namespace phenyl::core
