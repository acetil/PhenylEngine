#include "texture_manager.h"

#include "core/assets/assets.h"
#include "graphics/backend/renderer.h"
#include "graphics/detail/loggers.h"

using namespace phenyl::graphics;

static phenyl::Logger LOGGER{"TEXTURE_MANAGER", detail::GRAPHICS_LOGGER};

TextureManager::TextureManager (Renderer& renderer) : m_renderer{renderer} {}

std::shared_ptr<Texture> TextureManager::load (core::AssetLoadContext& ctx) {
    return ctx.withBinary().read([&] (std::istream& data) -> std::shared_ptr<Texture> {
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

        return std::make_shared<ImageTexture>(m_renderer.makeTexture(properties, *image));
    });
}

void TextureManager::selfRegister () {
    core::Assets::AddManager(this);
}
