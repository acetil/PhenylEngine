#pragma once

#include "asset.h"
#include "core/detail/loggers.h"
#include "core/serialization/backends.h"
#include "util/meta.h"

#include <functional>
#include <string>

namespace phenyl::core {
class AssetLoadContext {
public:
    AssetLoadContext& withExtension (std::string extension);
    AssetLoadContext& withBinary ();

    void read (const std::function<void(std::istream&)>& readFunc);

    template <AssetType T>
    std::shared_ptr<T> read (const std::function<std::shared_ptr<T>(std::istream&)>& readFunc) {
        std::shared_ptr<T> result = nullptr;
        read([&] (std::istream& data) { result = readFunc(data); });
        return result;
    }

    template <typename F>
    auto read (const F& func) -> decltype(func(std::declval<std::istream&>())) {
        return read(std::function{func});
    }

    template <AssetType T>
    std::shared_ptr<T> deserialize () {
        return read(std::function{[&] (std::istream& data) -> std::shared_ptr<T> {
            try {
                return std::make_shared<T>(core::DeserializeFromJson<T>(data));
            } catch (const DeserializeException& e) {
                PHENYL_LOGE(detail::ASSETS_LOGGER, "Failed to deserialize {}: {}", path(), e.what());
                return nullptr;
            }
        }});
    }

    template <AssetType T>
    std::shared_ptr<T> deserialize (ISerializable<T>& serializable) {
        return read(std::function{[&] (std::istream& data) -> std::shared_ptr<T> {
            try {
                auto ptr = std::make_shared<T>();
                core::DeserializeFromJson(data, serializable, *ptr);
                return ptr;
            } catch (const DeserializeException& e) {
                PHENYL_LOGE(detail::ASSETS_LOGGER, "Failed to deserialize {}: {}", path(), e.what());
                return nullptr;
            }
        }});
    }

    template <typename U, AssetType T>
    std::shared_ptr<T> deserialize (const std::function<std::shared_ptr<T>(U&&)>& mapFunc) {
        return read(std::function{[&] (std::istream& data) -> std::shared_ptr<T> {
            try {
                return mapFunc(core::DeserializeFromJson<U>(data));
            } catch (const DeserializeException& e) {
                PHENYL_LOGE(detail::ASSETS_LOGGER, "Failed to deserialize {}: {}", path(), e.what());
                return nullptr;
            }
        }});
    }

    template <typename F>
    requires (
        !std::derived_from<std::remove_cvref_t<F>, ISerializableBase> && !meta::IsStdFunction<std::remove_cvref_t<F>>)
    auto deserialize (const F& func) {
        return deserialize(std::function{func});
    }

    [[nodiscard]] std::string_view path () const noexcept {
        return m_path;
    }

    std::size_t id () const noexcept {
        return m_id;
    }

private:
    std::string m_path;
    std::string m_extension;
    std::size_t m_id;
    bool m_binary = false;

    explicit AssetLoadContext (std::string path, std::size_t id);

    friend class Assets;
};
} // namespace phenyl::core
