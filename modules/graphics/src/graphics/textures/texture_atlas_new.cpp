#include "graphics/textures/texture_atlas.h"
#include "build_atlas.h"

#define CHANNELS 4

using namespace phenyl::graphics;

static phenyl::Logger LOGGER{"TEXTURE_ATLAS", detail::GRAPHICS_LOGGER};

struct AtlasImage {
    std::size_t key;
    int width;
    int height;

    [[nodiscard]] std::size_t getKey () const {
        return key;
    }

    int getXSize () const {
        return width;
    }

    int getYSize () const {
        return height;
    }
};

static void fillData (std::byte* data, int size, const Image* image, int xOff, int yOff);

TextureAtlas::TextureAtlas (Renderer* renderer) : texture{renderer->makeImageTexture(TextureProperties{.format = ImageFormat::RGBA, .filter = TextureFilter::POINT, .useMipmapping = true})}, items{} {}

void TextureAtlas::build (const std::vector<Image*>& images) {
    std::vector<AtlasImage> atlasImages;
    atlasImages.reserve(images.size());

    for (std::size_t i = 0; i < images.size(); i++) {
        atlasImages.emplace_back(i, images[i]->width(), images[i]->height());
    }

    std::vector<AtlasObject<std::size_t>> objs;
    objs.reserve(images.size());
    int imgSize = buildAtlas(atlasImages.begin(), atlasImages.end(), std::back_inserter(objs), 1);

    if (imgSize <= 0) {
        PHENYL_LOGE(LOGGER, "Atlas build failed!");
        return;
    }

    PHENYL_LOGD(LOGGER, "Stitching atlas.");
    //auto data = std::make_unique<std::byte[]>(imgSize * imgSize * CHANNELS);
    Image atlasImage{static_cast<std::uint32_t>(imgSize), static_cast<std::uint32_t>(imgSize), ImageFormat::RGBA};
    items.clear();
    items.reserve(objs.size());
    std::fill_n(std::back_inserter(items), objs.size(), AtlasOffset{});

    for (auto& i : objs) {
        //fillData(data.get(), imgSize, images.at(i.key), i.xOff, i.yOff);
        atlasImage.blit(*images.at(i.key), {i.xOff, i.yOff});
        items.at(i.key) = AtlasOffset{glm::vec2{i.uOff, i.vOff}, glm::vec2 {i.uOff + i.uSize, i.vOff + i.vSize}};
    }

    //texture.reload(imgSize, imgSize, (unsigned char*)data.get());
    texture.upload(atlasImage);
    PHENYL_LOGD(LOGGER, "Stitching complete");
}

std::size_t TextureAtlas::size () const {
    return items.size();
}

const AtlasOffset& TextureAtlas::operator[] (std::size_t index) const {
    return items[index];
}

phenyl::util::Optional<const AtlasOffset&> TextureAtlas::at (std::size_t index) const {
    return index < items.size() ? util::Optional<const AtlasOffset&>{items[index]} : util::Optional<const AtlasOffset&>{};
}

const Texture& TextureAtlas::getTexture () const {
    return texture;
}

