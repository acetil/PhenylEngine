#include "common/assets/assets.h"

#include "graphics/textures/sprite_atlas.h"

using namespace phenyl::graphics;

SpriteTexture* SpriteAtlas::load (std::istream& data, std::size_t id) {
    PHENYL_DASSERT(!sprites.contains(id));
    rebuildRequired = true;
    auto imageOpt = Image::Load(data);
    if (!imageOpt) {
        return nullptr;
    }

    sprites[id] = std::make_unique<SpriteTexture>(std::move(imageOpt.getUnsafe()));
    return sprites[id].get();
}

const char* SpriteAtlas::getFileType () const {
    return ""; // TODO: other image formats
}

void SpriteAtlas::queueUnload (std::size_t id) {
    unloadQueue.push_back(id);
}

bool SpriteAtlas::isBinary () const {
    return true;
}

bool SpriteAtlas::rebuild () {
    if (!rebuildRequired) {
        return false;
    }

    for (auto i : unloadQueue) {
        if (onUnload(i)) {
            sprites.remove(i);
        }
    }
    unloadQueue.clear();

    std::vector<Image*> images;
    images.reserve(sprites.size());
    std::size_t index = 0;
    for (auto [k, v] : sprites.kv()) {
        images.push_back(&v->image);
        v->atlasIndex = index++;
    }

    atlas.build(images);
    for (auto [k, v] : sprites.kv()) {
        v->offset = atlas[v->atlasIndex];
    }

    rebuildRequired = false;
    return true;
}

SpriteAtlas::SpriteAtlas (Renderer* renderer) : atlas{renderer} {
    common::Assets::AddManager(this);
}

SpriteTexture* SpriteAtlas::load (SpriteTexture&& obj, std::size_t id) {
    sprites[id] = std::make_unique<SpriteTexture>(std::move(obj));
    return sprites[id].get();
}

const Texture& SpriteAtlas::getTexture () const {
    return atlas.getTexture();
}


