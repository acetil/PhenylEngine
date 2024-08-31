#include "common/assets/assets.h"

#include "graphics/detail/loggers.h"
#include "graphics/renderer.h"

#include "texture_manager.h"


using namespace phenyl::graphics;

static phenyl::Logger LOGGER{"TEXTURE_MANAGER", detail::GRAPHICS_LOGGER};

TextureManager::TextureManager (Renderer& renderer) : renderer{renderer} {}

Texture* TextureManager::load (std::ifstream& data, std::size_t id) {
    PHENYL_DASSERT(!textures.contains(id));

    auto image = Image::Load(data);
    if (!image) {
        PHENYL_LOGE(LOGGER, "Failed to load image from file!");
        return nullptr;
    }

    TextureProperties properties{
        .format = image.getUnsafe().format(),
        .filter = TextureFilter::POINT,
        .useMipmapping = true
    };

    textures.emplace(id, std::make_unique<ImageTexture>(renderer.makeTexture(properties, image.getUnsafe())));
    return textures[id].get();
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
    PHENYL_DASSERT(textures.contains(id));

    if (onUnload(id)) {
        textures.erase(id);
    }
}

void TextureManager::selfRegister () {
    common::Assets::AddManager(this);
}




