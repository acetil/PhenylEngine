#pragma once

#include <fstream>
#include <string>
#include <utility>

#include "util/fl_vector.h"
#include "util/map.h"
#include "util/meta.h"

#include "forward.h"
#include "asset.h"
#include "asset_manager.h"

namespace common {
    namespace detail {
        template <typename T>
        struct AssetEntry {
            T* data{nullptr};
            std::string path{};
            std::size_t refCount{0};
        };

        class AssetCacheBase {
        protected:
            AssetManagerBase* manager;
            explicit AssetCacheBase (AssetManagerBase* manager) : manager{manager} {}
        public:
            virtual ~AssetCacheBase () = default;
            virtual void incRefCount (std::size_t id) = 0;
            virtual bool decRefCount (std::size_t id) = 0;
            virtual bool removeEntry (std::size_t id) = 0;
            virtual std::string_view getPath (std::size_t id) = 0;

            [[nodiscard]] AssetManagerBase* getManagerBase () const {
                return manager;
            }
        };

        template <typename T>
        class AssetCache : public AssetCacheBase {
        private:
            static constexpr std::string_view UnknownPath = "";

            util::Map<std::string, std::size_t> pathMap;
            util::FLVector<AssetEntry<T>> cache;
        public:
            explicit AssetCache (AssetManager<T>* manager) : AssetCacheBase{manager}, pathMap{}, cache{} {}
            Asset<T> getCached (const std::string& path) {
                if (pathMap.contains(path)) {
                    auto index = pathMap.at(path);
                    auto& entry = cache.at(index);
                    entry.refCount++;

                    return Asset<T>{index + 1, entry.data};
                } else {
                    return Asset<T>{};
                }
            }

            AssetManager<T>* getManager () const {
                return static_cast<AssetManager<T>*>(manager);
            }

            std::size_t addEntry (const std::string& path) {
                assert(!pathMap.contains(path));
                auto index = cache.emplace(nullptr, path, 1);
                pathMap[path] = index;

                return index + 1;
            }

            void putData (std::size_t id, T* data) {
                assert(id && cache.present(id - 1));
                cache.at(id - 1).data = data;
            }

            void incRefCount (std::size_t id) override {
                assert(id && cache.present(id - 1));
                cache.at(id - 1).refCount++;
            }

            bool decRefCount (std::size_t id) override {
                assert(id && cache.present(id - 1));
                assert(cache.at(id - 1).refCount);
                return --cache.at(id - 1).refCount;
            }

            bool removeEntry (std::size_t id) override {
                assert(id && cache.present(id - 1));
                if (cache.at(id - 1).refCount) {
                    return false;
                }

                forceRemoveEntry(id);
                return true;
            }

            void forceRemoveEntry (std::size_t id) {
                assert(id && cache.present(id - 1));
                pathMap.remove(cache.at(id - 1).path);
                cache.remove(id - 1);
            }

            std::string_view getPath (std::size_t id) override {
                if (id && cache.present(id - 1)) {
                    return cache.at(id - 1).path;
                } else {
                    return UnknownPath;
                }
            }
        };
    }
    class Assets {
    private:
        static Assets* INSTANCE;

        static Assets* GetInstance () {
            if (!INSTANCE) {
                [[unlikely]]
                INSTANCE = new Assets();
            }

            return INSTANCE;
        }

        util::Map<std::size_t, std::unique_ptr<detail::AssetCacheBase>> caches;

        void incrementRefCount (std::size_t typeIndex, std::size_t id) {
            assert(caches.contains(typeIndex));
            caches.at(typeIndex)->incRefCount(id);
        }

        void decrementRefCount (std::size_t typeIndex, std::size_t id) {
            if (!caches.contains(typeIndex)) {
                logging::log(LEVEL_WARNING, "Attempted to decrement ref count of manager with type {} that no longer exists!", typeIndex);
                return;
            }
            auto& entry = caches.at(typeIndex);
            if (!entry->decRefCount(id)) {
                entry->getManagerBase()->queueUnload(id);
            }
        }
        bool unloadAsset (std::size_t typeIndex, std::size_t id) {
            assert(caches.contains(typeIndex));
            return caches.at(typeIndex)->removeEntry(id);
        }

        template <typename T>
        Asset<T> load (const std::string& path) {
            if (!caches.contains(meta::type_index<T>())) {
                logging::log(LEVEL_ERROR, "Attempted to load asset of unknown type!");
                return Asset<T>{};
            }

            auto* cache = static_cast<detail::AssetCache<T>*>(caches.at(meta::type_index<T>()).get());
            Asset<T> asset;
            if ((asset = std::move(cache->getCached(path)))) {
                if (!asset.get()) {
                    logging::log(LEVEL_FATAL, "Possible cyclic dependency including \"{}\"!", path);
                    assert(false);
                }
                return std::move(asset);
            }

            std::ifstream file;

            if (cache->getManagerBase()->isBinary()) {
                file = std::ifstream{path + cache->getManager()->getFileType(), std::ios::binary};
            } else {
                file = std::ifstream{path + cache->getManager()->getFileType()};
            }

            if (!file) {
                logging::log(LEVEL_ERROR, "Failed to open file at \"{}\"!", path + cache->getManager()->getFileType());
                return Asset<T>{};
            }

            auto id = cache->addEntry(path);
            T* ptr = cache->getManager()->load(file, id);
            if (!ptr) {
                logging::log(LEVEL_ERROR, "Failed to load asset at \"{}\"!", path);
                cache->forceRemoveEntry(id);
                return Asset<T>{};
            }
            cache->putData(id, ptr);

            return Asset<T>{id, ptr};
        }

        template <typename T>
        void addManager (AssetManager<T>* manager) {
            if (caches.contains(meta::type_index<T>())) {
                logging::log(LEVEL_ERROR, "Attempted to add asset manager that has already been added!");
                return;
            }

            caches[meta::type_index<T>()] = std::make_unique<detail::AssetCache<T>>(manager);
        }

        template <typename T>
        void removeManager (AssetManager<T>* manager) {
            if (!caches.contains(meta::type_index<T>())) {
                logging::log(LEVEL_ERROR, "Attempted to remove asset manager that does not exist!");
                return;
            }

            caches.remove(meta::type_index<T>());
        }

        std::string_view getPath (std::size_t typeIndex, std::size_t id) {
            assert(caches.contains(typeIndex));
            auto& cache = caches.at(typeIndex);
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
    public:
        template <typename T>
        static Asset<T> Load (const std::string& path) {
            return GetInstance()->load<T>(path);
        }

        template <typename T>
        static void AddManager (AssetManager<T>* manager) {
            GetInstance()->addManager(manager);
        }

        template <typename T>
        static void RemoveManager (AssetManager<T>* manager) {
            GetInstance()->removeManager(manager);
        }
    };
}