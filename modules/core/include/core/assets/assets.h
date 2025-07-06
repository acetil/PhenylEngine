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
    template <typename T>
    struct AssetEntry {
        T* data{nullptr};
        std::string path{};
        std::size_t refCount{0};
    };

    class AssetCacheBase {
    public:
        virtual ~AssetCacheBase () = default;
        virtual void incRefCount (std::size_t id) = 0;
        virtual bool decRefCount (std::size_t id) = 0;
        virtual bool removeEntry (std::size_t id) = 0;
        virtual std::string_view getPath (std::size_t id) = 0;

        [[nodiscard]] AssetManagerBase* getManagerBase () const {
            return m_manager;
        }

    protected:
        AssetManagerBase* m_manager;

        explicit AssetCacheBase (AssetManagerBase* manager) : m_manager{manager} {}
    };

    template <typename T>
    class AssetCache : public AssetCacheBase {
    public:
        explicit AssetCache (AssetManager<T>* manager) : AssetCacheBase{manager}, m_pathMap{}, m_cache{} {}

        Asset<T> getCached (const std::string& path) {
            if (m_pathMap.contains(path)) {
                auto index = m_pathMap.at(path);
                auto& entry = m_cache.at(index);
                entry.refCount++;

                return Asset<T>{index + 1, entry.data};
            } else {
                return Asset<T>{};
            }
        }

        AssetManager<T>* getManager () const {
            return static_cast<AssetManager<T>*>(m_manager);
        }

        std::size_t addEntry (const std::string& path) {
            PHENYL_DASSERT(!m_pathMap.contains(path));
            auto index = m_cache.emplace(nullptr, path, 1);
            m_pathMap[path] = index;

            return index + 1;
        }

        void putData (std::size_t id, T* data) {
            PHENYL_DASSERT(id && m_cache.present(id - 1));
            m_cache.at(id - 1).data = data;
        }

        void incRefCount (std::size_t id) override {
            PHENYL_DASSERT(id && m_cache.present(id - 1));
            m_cache.at(id - 1).refCount++;
        }

        bool decRefCount (std::size_t id) override {
            PHENYL_DASSERT(id && m_cache.present(id - 1));
            PHENYL_DASSERT(m_cache.at(id - 1).refCount);
            return --m_cache.at(id - 1).refCount;
        }

        bool removeEntry (std::size_t id) override {
            PHENYL_DASSERT(id && m_cache.present(id - 1));
            if (m_cache.at(id - 1).refCount) {
                return false;
            }

            forceRemoveEntry(id);
            return true;
        }

        void forceRemoveEntry (std::size_t id) {
            PHENYL_DASSERT(id && m_cache.present(id - 1));
            m_pathMap.erase(m_cache.at(id - 1).path);
            m_cache.remove(id - 1);
        }

        std::string_view getPath (std::size_t id) override {
            if (id && m_cache.present(id - 1)) {
                return m_cache.at(id - 1).path;
            } else {
                return UnknownPath;
            }
        }

    private:
        static constexpr std::string_view UnknownPath = "";

        std::unordered_map<std::string, std::size_t> m_pathMap;
        util::FLVector<AssetEntry<T>> m_cache;
    };

    class IAssetCache2 {
    public:
        virtual ~IAssetCache2 () = default;

        virtual void remove (std::size_t assetId) = 0;
    };

    template <typename T>
    class AssetCache2 : public IAssetCache2 {
    public:
        AssetCache2 (AssetManager<T>& manager) : m_manager{manager} {}

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

        std::shared_ptr<T> load (std::string_view path, std::ifstream& data) {
            PHENYL_DASSERT(!m_pathMap.contains(path));

            auto ptr = m_manager.load2(data);
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
    // template <typename T>
    // static Asset<T> Load (const std::string& path) {
    //     return GetInstance()->load<T>(path);
    // }

    template <IsAssetType T>
    static std::shared_ptr<T> Load2 (std::string_view path) {
        return GetInstance()->load2<T>(path);
    }

    // template <typename T>
    // static Asset<T> LoadVirtual (const std::string& virtualPath, T&& obj) {
    //     return GetInstance()->virtualLoad(virtualPath, std::forward<T>(obj));
    // }

    template <IsAssetType T>
    static void LoadVirtual2 (std::string_view path, const std::shared_ptr<T>& obj) {
        GetInstance()->virtualLoad2(path, obj);
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

    std::unordered_map<meta::TypeIndex, std::unique_ptr<detail::AssetCacheBase>> m_caches;
    std::unordered_map<meta::TypeIndex, std::unique_ptr<detail::IAssetCache2>> m_caches2;

    void incrementRefCount (meta::TypeIndex typeIndex, std::size_t id) {
        PHENYL_DASSERT(m_caches.contains(typeIndex));
        m_caches.at(typeIndex)->incRefCount(id);
    }

    void decrementRefCount (meta::TypeIndex typeIndex, std::size_t id) {
        if (!m_caches.contains(typeIndex)) {
            PHENYL_LOGE(detail::ASSETS_LOGGER,
                "Attempted to decrement ref count of manager with type {} that no longer "
                "exists!",
                typeIndex);
            return;
        }
        auto& entry = m_caches.at(typeIndex);
        if (!entry->decRefCount(id)) {
            entry->getManagerBase()->queueUnload(id);
        }
    }

    bool unloadAsset (meta::TypeIndex typeIndex, std::size_t id) {
        PHENYL_DASSERT(m_caches.contains(typeIndex));
        return m_caches.at(typeIndex)->removeEntry(id);
    }

    void unloadAsset2 (meta::TypeIndex type, std::size_t id) {
        PHENYL_DASSERT(m_caches2.contains(type));
        PHENYL_DASSERT(id);
        return m_caches2.at(type)->remove(id);
    }

    template <typename T>
    Asset<T> load (const std::string& path) {
        if (!m_caches.contains(meta::TypeIndex::Get<T>())) {
            PHENYL_LOGE(detail::ASSETS_LOGGER, "Attempted to load asset of unknown type!");
            return Asset<T>{};
        }

        auto* cache = static_cast<detail::AssetCache<T>*>(m_caches.at(meta::TypeIndex::Get<T>()).get());
        Asset<T> asset;
        if ((asset = std::move(cache->getCached(path)))) {
            PHENYL_ASSERT_MSG(asset.get(), "Possible cyclic dependency including \"{}\"!", path);

            return std::move(asset);
        }

        std::ifstream file;

        if (cache->getManagerBase()->isBinary()) {
            file = std::ifstream{path + cache->getManager()->getFileType(), std::ios::binary};
        } else {
            file = std::ifstream{path + cache->getManager()->getFileType()};
        }

        if (!file) {
            PHENYL_LOGE(detail::ASSETS_LOGGER, "Failed to open file at \"{}\"!",
                path + cache->getManager()->getFileType());
            return Asset<T>{};
        }

        auto id = cache->addEntry(path);
        T* ptr = cache->getManager()->load(file, id);
        if (!ptr) {
            PHENYL_LOGE(detail::ASSETS_LOGGER, "Failed to load asset at \"{}\"!", path);
            cache->forceRemoveEntry(id);
            return Asset<T>{};
        }
        cache->putData(id, ptr);

        if constexpr (std::derived_from<T, IAssetType<T>>) {
            static_cast<IAssetType<T>*>(ptr)->m_id = id;
        }

        return Asset<T>{id, ptr};
    }

    template <IsAssetType T>
    std::shared_ptr<T> load2 (std::string_view path) {
        auto type = meta::TypeIndex::Get<T>();
        auto cacheIt = m_caches2.find(type);
        if (cacheIt == m_caches2.end()) {
            PHENYL_LOGE(detail::ASSETS_LOGGER, "Attempted to load asset of unknown type!");
            return nullptr;
        }
        detail::AssetCache2<T>& cache = static_cast<detail::AssetCache2<T>&>(*cacheIt->second);

        if (auto ptr = cache.lookup(path)) {
            return ptr;
        }

        std::ifstream file;
        if (cache.manager().isBinary()) {
            file = std::ifstream{std::string{path} + cache.manager().getFileType(), std::ios::binary};
        } else {
            file = std::ifstream{std::string{path} + cache.manager().getFileType()};
        }
        if (!file) {
            PHENYL_LOGE(detail::ASSETS_LOGGER, "Failed to open file at \"{}{}\"!", path, cache.manager().getFileType());
            return nullptr;
        }

        auto ptr = cache.load(path, file);
        PHENYL_LOGE_IF(!ptr, detail::ASSETS_LOGGER, "Failed to load asset at \"{}\"!", path);
        return ptr;
    }

    template <typename T>
    Asset<T> virtualLoad (const std::string& path, T&& obj) {
        if (!m_caches.contains(meta::TypeIndex::Get<T>())) {
            PHENYL_LOGE(detail::ASSETS_LOGGER, "Attempted to load asset of unknown type!");
            return Asset<T>{};
        }

        auto* cache = static_cast<detail::AssetCache<T>*>(m_caches.at(meta::TypeIndex::Get<T>()).get());

        PHENYL_ASSERT_MSG(!cache->getCached(path), "Attempted to virtual load an asset multiple times!", path);

        auto id = cache->addEntry(path);
        T* ptr = cache->getManager()->load(std::forward<T>(obj), id);
        if (!ptr) {
            PHENYL_LOGE(detail::ASSETS_LOGGER, "Failed to load asset at \"{}\"!", path);
            cache->forceRemoveEntry(id);
            return Asset<T>{};
        }
        cache->putData(id, ptr);

        return Asset<T>{id, ptr};
    }

    template <IsAssetType T>
    void virtualLoad2 (std::string_view path, const std::shared_ptr<T>& obj) {
        auto type = meta::TypeIndex::Get<T>();
        auto cacheIt = m_caches2.find(type);
        if (cacheIt == m_caches2.end()) {
            PHENYL_LOGE(detail::ASSETS_LOGGER, "Attempted to virtually load asset of unknown type!");
            return;
        }
        detail::AssetCache2<T>& cache = static_cast<detail::AssetCache2<T>&>(*cacheIt->second);

        cache.virtualLoad(path, obj);
    }

    template <IsAssetType T>
    void addManager (AssetManager<T>* manager) {
        if (m_caches.contains(meta::TypeIndex::Get<T>())) {
            PHENYL_LOGE(detail::ASSETS_LOGGER, "Attempted to add asset manager that has already been added!");
            return;
        }

        m_caches[meta::TypeIndex::Get<T>()] = std::make_unique<detail::AssetCache<T>>(manager);
        m_caches2.emplace(meta::TypeIndex::Get<T>(), std::make_unique<detail::AssetCache2<T>>(*manager));
    }

    template <IsAssetType T>
    void removeManager (AssetManager<T>* manager) {
        if (!m_caches.contains(meta::TypeIndex::Get<T>())) {
            PHENYL_LOGE(detail::ASSETS_LOGGER, "Attempted to remove asset manager that does not exist!");
            return;
        }

        m_caches.erase(meta::TypeIndex::Get<T>());
        m_caches2.erase(meta::TypeIndex::Get<T>());
    }

    std::string_view getPath (meta::TypeIndex typeIndex, std::size_t id) {
        PHENYL_DASSERT(m_caches.contains(typeIndex));
        auto& cache = m_caches.at(typeIndex);
        return cache->getPath(id);
    }

    static void IncrementRefCount (meta::TypeIndex typeIndex, std::size_t id) {
        GetInstance()->incrementRefCount(typeIndex, id);
    }

    static void DecrementRefCount (meta::TypeIndex typeIndex, std::size_t id) {
        GetInstance()->decrementRefCount(typeIndex, id);
    }

    static bool UnloadAsset (meta::TypeIndex typeIndex, std::size_t id) {
        return GetInstance()->unloadAsset(typeIndex, id);
    }

    static void OnUnload2 (meta::TypeIndex type, std::size_t id) {
        GetInstance()->unloadAsset2(type, id);
    }

    static std::string_view GetPath (meta::TypeIndex typeIndex, std::size_t id) {
        return GetInstance()->getPath(typeIndex, id);
    }

    friend detail::AssetManagerBase;
    friend detail::AssetBase;
    friend AssetTypeUntyped;
};

namespace detail {
    // template <typename T>
    // class AssetSerializable : public ISerializable<Asset<T>> {
    // public:
    //     std::string_view name () const noexcept override {
    //         return "Asset";
    //     }
    //
    //     void serialize (ISerializer& serializer, const Asset<T>& obj) override {
    //         serializer.serialize(obj.id());
    //     }
    //
    //     void deserialize (IDeserializer& deserializer, Asset<T>& obj) override {
    //         deserializer.deserializeString(*this, obj);
    //     }
    //
    //     void deserializeString (Asset<T>& obj, std::string_view string) override {
    //         obj = Assets::Load<T>(std::string{string}); // TODO
    //         if (!obj) {
    //             throw DeserializeException(std::format("Failed to load asset at \"{}\"", string));
    //         }
    //     }
    // };

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
            obj = Assets::Load2<T>(string); // TODO
            if (!obj) {
                throw DeserializeException(std::format("Failed to load asset at \"{}\"", string));
            }
        }
    };
} // namespace detail

// template <typename T>
// ISerializable<Asset<T>>& phenyl_GetSerializable (detail::SerializableMarker<Asset<T>>) {
//     static detail::AssetSerializable<T> serializable;
//     return serializable;
// }

template <IsAssetType T>
ISerializable<std::shared_ptr<T>>& phenyl_GetSerializable (detail::SerializableMarker<std::shared_ptr<T>>) {
    static detail::AssetSerializable2<T> serializable;
    return serializable;
}
} // namespace phenyl::core
