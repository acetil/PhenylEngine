#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <math.h>

#include "texture_atlas.h"
#include "image.h"
#include "texture.h"
#include "logging/logging.h"

#define BYTES_PER_PIXEL 4
using namespace graphics;
typedef struct _node *Node;
typedef struct _node {
    int width;
    int height;
    int offsetX;
    int offsetY;
    Node leftNode = NULL;
    Node rightNode = NULL;
    Image* image = NULL;
    void setDimensions (int width, int height, int offsetX, int offsetY);
    int getArea ();
    bool insert (Image* i);
    void destroy ();
    std::vector<Node> walk ();
    void writeData (unsigned char* data, int sideLength);
} node;
bool insertImages (Node tree, std::vector<Image*> images) {
    // returns true if all images fit in
    for (Image* i : images) {
        if (!tree->insert(i)) {
            return false;
        }
    }
    return true;
}
void graphics::TextureAtlas::createAtlas (std::vector<Image*> images) {
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
}

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

void graphics::TextureAtlas::loadTextureAtlas () {
    glGenTextures(1, &glTextureId);

    glBindTexture(GL_TEXTURE_2D, glTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sideLength, sideLength, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    // mipmapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    logging::logf(LEVEL_INFO, "Generating mipmaps for %d * %d texture atlas", sideLength, sideLength);
    glGenerateMipmap(GL_TEXTURE_2D);
}

void graphics::TextureAtlas::bindTextureAtlas () {
    glBindTexture(GL_TEXTURE_2D, glTextureId);
}

void node::setDimensions (int width, int height, int offsetX, int offsetY) {
    this->width = width;
    this->height = height;
    this->offsetX = offsetX;
    this->offsetY = offsetY;
}

int node::getArea () {
    return width * height;
}
void node::destroy () {
    if (leftNode != NULL) {
        leftNode->destroy();
        delete leftNode;
    }
    if (rightNode != NULL) {
        rightNode->destroy();
        delete rightNode;
    }
    image = NULL;
}
bool node::insert (Image* i) {
    // recursively uses a depth-first search for a space which is bigger than the image
    // returns true if a space can be found, false otherwise
    // see http://blackpawn.com/texts/lightmaps/ for algorithm, which has been very slightly tweaked
    if (image != NULL) {
        // image already present
        if (leftNode != NULL && rightNode != NULL) {
            // should always get here
            return leftNode->insert(i) || rightNode->insert(i);
        } else {
            // just in case
            return false;
        }
    }
    if (width >= i->getWidth() && height >= i->getHeight()) {
        // image can be inserted

        // split on x axis first
        leftNode = new node;
        leftNode->setDimensions(width - i->getWidth(), height, offsetX + i->getWidth(), offsetY);

        // then split on y
        rightNode = new node;
        rightNode->setDimensions(i->getWidth(), height - i->getHeight(), 
            offsetX, offsetY + i->getHeight());
        
        width = i->getWidth();
        height = i->getHeight();
        image = i;
        return true;
    } else {
        return false;
    }
}
void node::writeData (unsigned char* data, int sideLength) {
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
    if (this->image != NULL) {
        nodes.push_back(this);
    }
    if (leftNode != NULL) {
        std::vector<Node> nodeInput = leftNode->walk();
        nodes.insert(nodes.end(), nodeInput.begin(), nodeInput.end());
    }
    if (rightNode != NULL) {
        std::vector<Node> nodeInput = rightNode->walk();
        nodes.insert(nodes.end(), nodeInput.begin(), nodeInput.end());
    }
    return nodes;
}
