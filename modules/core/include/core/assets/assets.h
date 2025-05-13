#pragma once

#include "asset.h"
#include "asset_manager.h"
#include "core/detail/loggers.h"
#include "forward.h"
#include "util/fl_vector.h"
#include "util/map.h"
#include "util/meta.h"

#include <fstream>
#include <string>
#include <utility>

namespace phenyl::core {
namespace detail {
    template<typename T>
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

    template<typename T>
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
            m_pathMap.remove(m_cache.at(id - 1).path);
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

        util::Map<std::string, std::size_t> m_pathMap;
        util::FLVector<AssetEntry<T>> m_cache;
    };
} // namespace detail

class Assets {
public:
    template<typename T>
    static Asset<T> Load (const std::string& path) {
        return GetInstance()->load<T>(path);
    }

    template<typename T>
    static Asset<T> LoadVirtual (const std::string& virtualPath, T&& obj) {
        return GetInstance()->virtualLoad(virtualPath, std::forward<T>(obj));
    }

    template<typename T>
    static void AddManager (AssetManager<T>* manager) {
        GetInstance()->addManager(manager);
    }

    template<typename T>
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

    util::Map<std::size_t, std::unique_ptr<detail::AssetCacheBase>> m_caches;

    void incrementRefCount (std::size_t typeIndex, std::size_t id) {
        PHENYL_DASSERT(m_caches.contains(typeIndex));
        m_caches.at(typeIndex)->incRefCount(id);
    }

    void decrementRefCount (std::size_t typeIndex, std::size_t id) {
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

    bool unloadAsset (std::size_t typeIndex, std::size_t id) {
        PHENYL_DASSERT(m_caches.contains(typeIndex));
        return m_caches.at(typeIndex)->removeEntry(id);
    }

    template<typename T>
    Asset<T> load (const std::string& path) {
        if (!m_caches.contains(meta::type_index<T>())) {
            PHENYL_LOGE(detail::ASSETS_LOGGER, "Attempted to load asset of unknown type!");
            return Asset<T>{};
        }

        auto* cache = static_cast<detail::AssetCache<T>*>(m_caches.at(meta::type_index<T>()).get());
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

    template<typename T>
    Asset<T> virtualLoad (const std::string& path, T&& obj) {
        if (!m_caches.contains(meta::type_index<T>())) {
            PHENYL_LOGE(detail::ASSETS_LOGGER, "Attempted to load asset of unknown type!");
            return Asset<T>{};
        }

        auto* cache = static_cast<detail::AssetCache<T>*>(m_caches.at(meta::type_index<T>()).get());

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

    template<typename T>
    void addManager (AssetManager<T>* manager) {
        if (m_caches.contains(meta::type_index<T>())) {
            PHENYL_LOGE(detail::ASSETS_LOGGER, "Attempted to add asset manager that has already been added!");
            return;
        }

        m_caches[meta::type_index<T>()] = std::make_unique<detail::AssetCache<T>>(manager);
    }

    template<typename T>
    void removeManager (AssetManager<T>* manager) {
        if (!m_caches.contains(meta::type_index<T>())) {
            PHENYL_LOGE(detail::ASSETS_LOGGER, "Attempted to remove asset manager that does not exist!");
            return;
        }

        m_caches.remove(meta::type_index<T>());
    }

    std::string_view getPath (std::size_t typeIndex, std::size_t id) {
        PHENYL_DASSERT(m_caches.contains(typeIndex));
        auto& cache = m_caches.at(typeIndex);
        return cache->getPath(id);
    }

    static void IncrementRefCount (std::size_t typeIndex, std::size_t id) {
        GetInstance()->incrementRefCount(typeIndex, id);
    }

    static void DecrementRefCount (std::size_t typeIndex, std::size_t id) {
        GetInstance()->decrementRefCount(typeIndex, id);
    }

    static bool UnloadAsset (std::size_t typeIndex, std::size_t id) {
        return GetInstance()->unloadAsset(typeIndex, id);
    }

    static std::string_view GetPath (std::size_t typeIndex, std::size_t id) {
        return GetInstance()->getPath(typeIndex, id);
    }

    friend detail::AssetManagerBase;
    friend detail::AssetBase;
};

namespace detail {
    template<typename T>
    class AssetSerializable : public ISerializable<Asset<T>> {
    public:
        std::string_view name () const noexcept override {
            return "Asset";
        }

        void serialize (ISerializer& serializer, const Asset<T>& obj) override {
            serializer.serialize(obj.id());
        }

        void deserialize (IDeserializer& deserializer, Asset<T>& obj) override {
            deserializer.deserializeString(*this, obj);
        }

        void deserializeString (Asset<T>& obj, std::string_view string) override {
            obj = Assets::Load<T>(std::string{string}); // TODO
            if (!obj) {
                throw DeserializeException(std::format("Failed to load asset at \"{}\"", string));
            }
        }
    };
} // namespace detail

template<typename T>
ISerializable<Asset<T>>& phenyl_GetSerializable (detail::SerializableMarker<Asset<T>>) {
    static detail::AssetSerializable<T> serializable;
    return serializable;
}
} // namespace phenyl::core
