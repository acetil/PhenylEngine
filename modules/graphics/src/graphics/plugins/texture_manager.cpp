#include "texture_manager.h"

#include "core/assets/assets.h"
#include "graphics/backend/renderer.h"
#include "graphics/detail/loggers.h"

using namespace phenyl::graphics;

static phenyl::Logger LOGGER{"TEXTURE_MANAGER", detail::GRAPHICS_LOGGER};

TextureManager::TextureManager (Renderer& renderer) : m_renderer{renderer} {}

Texture* TextureManager::load (std::ifstream& data, std::size_t id) {
    PHENYL_DASSERT(!m_textures.contains(id));

    auto image = Image::Load(data);
    if (!image) {
        PHENYL_LOGE(LOGGER, "Failed to load image from file!");
        return nullptr;
    }

    TextureProperties properties{
      .format = image->format(),
      .filter = TextureFilter::POINT,
      .useMipmapping = true,
    };

    m_textures.emplace(id, std::make_unique<ImageTexture>(m_renderer.makeTexture(properties, *image)));
    return m_textures[id].get();
}

Texture* TextureManager::load (Texture&& obj, std::size_t id) {
    PHENYL_ABORT("Virtual loading of textures not supported!");
}

bool TextureManager::isBinary () const {
    return true;
}

const char* TextureManager::getFileType () const {
    return "";
}

void TextureManager::queueUnload (std::size_t id) {
    PHENYL_DASSERT(m_textures.contains(id));

    if (onUnload(id)) {
        m_textures.erase(id);
    }
}

void TextureManager::selfRegister () {
    core::Assets::AddManager(this);
}
