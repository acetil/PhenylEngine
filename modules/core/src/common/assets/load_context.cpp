#include "core/assets/load_context.h"

#include "core/detail/loggers.h"

#include <fstream>

using namespace phenyl::core;

AssetLoadContext::AssetLoadContext (std::string path, std::size_t id) : m_path{std::move(path)}, m_id{id} {}

AssetLoadContext& AssetLoadContext::withExtension (std::string extension) {
    m_extension = std::move(extension);
    return *this;
}

AssetLoadContext& AssetLoadContext::withBinary () {
    m_binary = true;
    return *this;
}

void AssetLoadContext::read (const std::function<void(std::istream&)>& readFunc) {
    std::string path = !m_extension.empty() ? m_path + m_extension : m_path;
    std::ifstream stream{path, m_binary ? std::ios::binary : std::ios::in};
    if (!stream) {
        PHENYL_LOGE(detail::ASSETS_LOGGER, "Failed to open file at \"{}\"!", path);
        return;
    }
    readFunc(stream);
}
