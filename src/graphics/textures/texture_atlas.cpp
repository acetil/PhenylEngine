#include <vector>
#include <algorithm>
#include <unordered_set>

#include "texture_atlas.h"
#include "image.h"
#include "build_atlas.h"
#include "logging/logging.h"

#define BYTES_PER_PIXEL 4
#define PADDING 1

#define TRIANGLES_PER_IMAGE 2
#define VERTICES_PER_TRIANGLE 3
#define COMP_PER_VERTEX 2

using namespace graphics;
typedef struct node *Node;
struct node {
    int width = 0;
    int height = 0;
    int offsetX = 0;
    int offsetY = 0;
    Node leftNode = nullptr;
    Node rightNode = nullptr;
    Image* image = nullptr;
    void setDimensions (int width, int height, int offsetX, int offsetY);

    bool insert (Image* i);
    void destroy ();
    std::vector<Node> walk ();
    void writeData (unsigned char* data, int sideLength) const;
};

struct ImageContainer {
    Image* img;
    int key;
    ImageContainer (Image* _img, int _key) : img(_img), key(_key) {};
    int getKey () {
        return key;
    }
    int getXSize () {
        return img->getWidth();
    }
    int getYSize () {
        return img->getHeight();
    }
};

glm::vec2 getVertexVec (int index);
static void writeData (unsigned char* data, int sideLength, Image* img, AtlasObject<int> obj);
bool insertImages (Node tree, const std::vector<Image*>& images) {
    // returns true if all images fit in
    for (Image* i : images) {
        if (!tree->insert(i)) {
            return false;
        }
    }
    return true;
}
/*void graphics::TextureAtlas::createAtlas (std::vector<Image*> images) {
    // creates a texture atlas by first assigning each image to a space on a square
    // power of 2 texture using a greedy depth-first search

    // sorts to get larger images first
    std::sort(images.begin(), images.end(), [](Image* a, Image* b) {
        return a->getArea() > b->getArea();
    });
    int totalArea = 0;    
    for (Image* i : images) {
        totalArea += i->getArea();
    }
    logging::logf(LEVEL_INFO, "Initiated creation of texture atlas of %d images of total area %d.", 
        images.size(), totalArea);
    
    //getting initial atlas area side length
    int sideLength = 2;
    while (sideLength * sideLength < totalArea) {
        sideLength *= 2;
    }

    Node tree = new node;
    tree->setDimensions(sideLength, sideLength, 0, 0);
    logging::logf(LEVEL_INFO, "Attempting packing of %d * %d atlas.", sideLength, sideLength);
    while (!insertImages(tree, images)) {
        // space isn't big enough
        tree->destroy();
        sideLength *= 2;
        tree->setDimensions(sideLength, sideLength, 0, 0);
        logging::logf(LEVEL_INFO, "Unable to pack, attempting packing of %d * %d atlas.", sideLength, sideLength);
    }
    this->sideLength = sideLength;
    logging::log(LEVEL_INFO, "Packing success.");

    data = new unsigned char[sideLength * sideLength * BYTES_PER_PIXEL];
    textures = new Texture[images.size()];
    Texture* texPtr = textures;

    std::vector<Node> nodes = tree->walk();
    logging::log(LEVEL_INFO, "Stitching atlas.");
    for (Node n : nodes) {
        n->writeData(data, sideLength);
        texPtr->setName(n->image->getName());
        texPtr->setTexUvs(n->offsetX, n->offsetY, n->width, n->height, sideLength, sideLength);
        textureIdMap[texPtr->getName()] = texPtr - textures;
        texPtr++;
    }
    logging::log(LEVEL_INFO, "Stitching complete");
}*/
void graphics::TextureAtlas::createAtlas (const std::vector<Model>& modelsIn) {
    // TODO: cleanup
    // creates a texture atlas by first assigning each image to a space on a square
    // power of 2 texture using a greedy depth-first search

    // sorts to get larger images first
    std::unordered_set<Image*> imageSet;
    for (const auto& m : modelsIn) {
        for (const auto& i : m.textures) {
            imageSet.insert(i.second.get());
        }
    }
    auto images = std::vector(imageSet.begin(), imageSet.end());
    /*std::sort(images.begin(), images.end(), [](Image* a, Image* b) {
        return a->getArea() > b->getArea();
    });
    int totalArea = 0;
    for (Image* i : images) {
        totalArea += i->getArea();
    }
    logging::log(LEVEL_INFO, "Initiated creation of texture atlas of {} images of total area {}.",
                  images.size(), totalArea);

    //getting initial atlas area side length
    int length = 2;
    while (length * length < totalArea) {
        length *= 2;
    }

    Node tree = new node;
    tree->setDimensions(length, length, 0, 0);
    logging::log(LEVEL_INFO, "Attempting packing of {} * {} atlas.", length, length);
    while (!insertImages(tree, images)) {
        // space isn't big enough
        tree->destroy();
        length *= 2;
        tree->setDimensions(length, length, 0, 0);
        logging::log(LEVEL_INFO, "Unable to pack, attempting packing of {} * {} atlas.", length, length);
    }
    this->sideLength = length;
    logging::log(LEVEL_INFO, "Packing success.");
    */
    std::vector<ImageContainer> imgConts;
    for (size_t i = 0; i < images.size(); i++) {
        imgConts.emplace_back(ImageContainer(images[i], i));
    }
    std::vector<AtlasObject<int>> objs;
    int length = buildAtlas(imgConts.begin(), imgConts.end(), std::back_inserter(objs), PADDING);
    this->sideLength = length;
    data = std::shared_ptr<unsigned char[]> (new unsigned char[length * length * BYTES_PER_PIXEL]);
    //textures = new Texture[images.size()];
    //Texture* texPtr = textures;
    auto tempUvData = std::unique_ptr<float[]>(new float[images.size() * TRIANGLES_PER_IMAGE * VERTICES_PER_TRIANGLE * COMP_PER_VERTEX]);
    auto uvPtr = tempUvData.get();
    //std::vector<Node> nodes = tree->walk();
    logging::log(LEVEL_INFO, "Stitching atlas.");

    std::unordered_map<Image*, util::span<float>> imageMap;
    /*for (Node n : nodes) {
        n->writeData(data, length);
        auto originalUv = uvPtr;
        for (int i = 0; i < TRIANGLES_PER_IMAGE * VERTICES_PER_TRIANGLE; i++) {
            glm::vec2 offVec = getVertexVec(i);
            offVec.x = offVec.x * (float)n->width / (float)length + (float)n->offsetX / (float)length;
            offVec.y = offVec.y * (float) n->height / float(length) + (float)n->offsetY / (float) length;
            *(uvPtr++) = offVec[0];
            *(uvPtr++) = offVec[1];
        }
        imageMap[n->image] = util::span(originalUv, uvPtr);
    }*/
    for (auto obj : objs) {
        auto img = imgConts[obj.key].img;
        writeData(data.get(), length, img, obj);
        auto originalUv = uvPtr;
        for (int i = 0; i < TRIANGLES_PER_IMAGE * VERTICES_PER_TRIANGLE; i++) {
            glm::vec2 offVec = getVertexVec(i);
            offVec.x = offVec.x * obj.uSize + obj.uOff;
            offVec.y = offVec.y * obj.vSize + obj.vOff;
            *(uvPtr++) = offVec[0];
            *(uvPtr++) = offVec[1];
        }
        imageMap[img] = util::span(originalUv, uvPtr);
    }


    int numModelComponents = 0;
    for (const auto& m : modelsIn) {
        numModelComponents += m.textures.size();
    }

    positionData = std::shared_ptr<float[]>(new float[TRIANGLES_PER_IMAGE * VERTICES_PER_TRIANGLE * COMP_PER_VERTEX * numModelComponents]);
    uvData = std::shared_ptr<float[]>(new float[TRIANGLES_PER_IMAGE * VERTICES_PER_TRIANGLE * COMP_PER_VERTEX * numModelComponents]);
    auto posPtr = positionData.get();
    uvPtr = uvData.get();
    for (const auto& m : modelsIn) {
        auto originalPos = posPtr;
        auto originalUv = uvPtr;
        for (auto pair : m.textures) {
            for (int i = 0; i < TRIANGLES_PER_IMAGE * VERTICES_PER_TRIANGLE; i++) {
                glm::vec2 offVec = getVertexVec(i) * 2.0f - glm::vec2{1.0f, 1.0f};
                glm::vec2 finalVec = pair.first.recMat *  offVec + pair.first.offset;
                *(posPtr++) = finalVec.x;
                *(posPtr++) = finalVec.y * -1.0f;
            }
            auto uvSpan = imageMap[pair.second.get()];
            memcpy(uvPtr, uvSpan.begin(), uvSpan.size() * sizeof(float));
            uvPtr += uvSpan.size();
        }
        models.emplace_back(FixedModel(originalPos, posPtr, originalUv, uvPtr, m.modelName));
        modelIdMap[m.modelName] = models.size() - 1;
    }

    logging::log(LEVEL_INFO, "Stitching complete");
    //tree->destroy();
    //delete tree;
}
/*
int graphics::TextureAtlas::getTextureId (std::string name) {
    return textureIdMap[name];
}

Texture* graphics::TextureAtlas::getTexture (int textureId) {
    return textures + textureId;
}

Texture* graphics::TextureAtlas::getTexture (std::string name) {
    // convenience function
    return getTexture(getTextureId(name));
}
*/
void graphics::TextureAtlas::bindTextureAtlas () {
    graphicsTexture.bindTexture();
}

void TextureAtlas::loadTextureAtlas (Renderer* renderer) {
    graphicsTexture = renderer->loadTexture(sideLength, sideLength, data.get());
    //delete[] data;
}

FixedModel TextureAtlas::getModel (int modelId) {
    return models[modelId];
}

[[maybe_unused]] FixedModel TextureAtlas::getModel (const std::string& name) {
    return models[modelIdMap[name]];
}

int TextureAtlas::getModelId (const std::string& name) {
    return modelIdMap[name];
}

void node::setDimensions (int _width, int _height, int _offsetX, int _offsetY) {
    this->width = _width;
    this->height = _height;
    this->offsetX = _offsetX;
    this->offsetY = _offsetY;
}

void node::destroy () {
    if (leftNode != nullptr) {
        leftNode->destroy();
        delete leftNode;
    }
    if (rightNode != nullptr) {
        rightNode->destroy();
        delete rightNode;
    }
    image = nullptr;
}
bool node::insert (Image* i) {
    // recursively uses a depth-first search for a space which is bigger than the image
    // returns true if a space can be found, false otherwise
    // see http://blackpawn.com/texts/lightmaps/ for algorithm, which has been very slightly tweaked
    if (image != nullptr) {
        // image already present
        if (leftNode != nullptr && rightNode != nullptr) {
            // should always get here
            return leftNode->insert(i) || rightNode->insert(i);
        } else {
            // just in case
            return false;
        }
    }
    if (width >= i->getWidth() + PADDING * 2 && height >= i->getHeight() + PADDING * 2) {
        // image can be inserted

        // split on x axis first
        leftNode = new node;
        leftNode->setDimensions(width - i->getWidth() - PADDING * 2, height, offsetX + i->getWidth() + PADDING * 2, offsetY);

        // then split on y
        rightNode = new node;
        rightNode->setDimensions(i->getWidth(), height - i->getHeight() - PADDING * 2, 
            offsetX, offsetY + i->getHeight() + PADDING * 2);
        
        width = i->getWidth();
        height = i->getHeight();
        image = i;
        offsetX += PADDING;
        offsetY += PADDING;
        return true;
    } else {
        return false;
    }
}
void node::writeData (unsigned char* data, int sideLength) const {
    unsigned char* copyPtr = data + (offsetX + offsetY * sideLength) * BYTES_PER_PIXEL;
    unsigned char* dataPtr = image->getData();
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            for (int k = 0; k < BYTES_PER_PIXEL; k++) {
                copyPtr[j * BYTES_PER_PIXEL + k] = *(dataPtr++);
            }
        }
        copyPtr += sideLength * BYTES_PER_PIXEL; // incrementing row
    }
}
std::vector<Node> node::walk () {
    std::vector<Node> nodes;
    if (this->image != nullptr) {
        nodes.push_back(this);
    }
    if (leftNode != nullptr) {
        std::vector<Node> nodeInput = leftNode->walk();
        nodes.insert(nodes.end(), nodeInput.begin(), nodeInput.end());
    }
    if (rightNode != nullptr) {
        std::vector<Node> nodeInput = rightNode->walk();
        nodes.insert(nodes.end(), nodeInput.begin(), nodeInput.end());
    }
    return nodes;
}

glm::vec2 getVertexVec (int index) {
    // for 0 1
    //     2 3
    // order is 0,1,2,3,1,2
    glm::vec2 vector = {0, 0};
    unsigned int correctedVertex = 3 * (index == 3) + index % 3;
    vector.x += (float) (correctedVertex & (unsigned int)1);
    vector.y += (float) ((correctedVertex & (unsigned int)2) >> (unsigned int) 1);
    return vector;
}

static void writeData (unsigned char* data, int sideLength, Image* img, AtlasObject<int> obj) {
    unsigned char* copyPtr = data + (obj.xOff + obj.yOff * sideLength) * BYTES_PER_PIXEL;
    unsigned char* dataPtr = img->getData();
    for (int i = 0; i < img->getHeight(); i++) {
        for (int j = 0; j < img->getWidth(); j++) {
            for (int k = 0; k < BYTES_PER_PIXEL; k++) {
                copyPtr[j * BYTES_PER_PIXEL + k] = *(dataPtr++);
            }
        }
        copyPtr += sideLength * BYTES_PER_PIXEL; // incrementing row
    }
}