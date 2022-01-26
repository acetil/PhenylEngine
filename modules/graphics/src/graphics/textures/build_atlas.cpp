#include <queue>
#include <utility>
#include <stdio.h>
#include "math.h"

#include "build_atlas.h"
#include "logging/logging.h"

using namespace graphics;

struct Node {
    int width = 0;
    int height = 0;
    int offsetX = 0;
    int offsetY = 0;
    Node* leftNode = nullptr;
    Node* rightNode = nullptr;
    int index = -1;
    void setDimensions (int _width, int _height, int _offsetX, int _offsetY);
    bool insert (atlas_internal::InternalAtlasImage& img, int padding);
    void destroy ();
    void walk (std::back_insert_iterator<std::vector<Node*>> out);

};

static void printTree (Node* root);

bool insertImages (Node* tree, const std::vector<atlas_internal::InternalAtlasImage>& images, int padding) {
    // returns true if all images fit in
    for (auto i : images) {
        //logging::log(LEVEL_DEBUG, "Inserting {} x {} image!", i.xSize, i.ySize);
        if (!tree->insert(i, padding)) {
            return false;
        }
        //printTree(tree);
    }
    return true;
}

std::pair<std::vector<AtlasObject<int>>, int> atlas_internal::buildAtlasInternal (std::vector<InternalAtlasImage>& imgs,
                                                                                  int padding) {
    // creates a texture atlas by first assigning each image to a space on a square
    // power of 2 texture using a greedy depth-first search

    // sorts to get larger images first
    std::sort(imgs.begin(), imgs.end(), [] (auto a, auto b) {
        //return (a.xSize > a.ySize ? a.xSize : a.ySize) > (b.xSize > b.ySize ? b.xSize : b.ySize);
        return a.xSize * a.ySize > b.xSize * b.ySize;
    });

    int totalArea = 0;
    for (auto i : imgs) {
        totalArea += i.xSize * i.ySize;
    }

    //getting initial atlas area side length
    int length = 2;
    while (length * length < totalArea) {
        length *= 2;
    }

    logging::log(LEVEL_INFO, "Initiated creation of atlas of {} images of total area {}.",
                 imgs.size(), totalArea);

    Node* tree = new Node();
    tree->setDimensions(length, length, 0, 0);
    logging::log(LEVEL_INFO, "Attempting packing of {} * {} atlas.", length, length);
    while (!insertImages(tree, imgs, padding)) {
        // space isn't big enough
        tree->destroy();
        length *= 2;
        tree->setDimensions(length, length, 0, 0);
        logging::log(LEVEL_INFO, "Unable to pack, attempting packing of {} * {} atlas.", length, length);
    }

    std::vector<Node*> nodes;
    tree->walk(std::back_inserter(nodes));

    std::vector<AtlasObject<int>> objs;
    objs.reserve(nodes.size());
    for (auto n : nodes) {
        objs.emplace_back(n->offsetX, n->offsetY, (float) n->offsetX / (float) length, (float) n->offsetY / (float)length, (
                float) n->width / (float) length, (float) n->height / (float) length, n->index);
    }

    tree->destroy();

    delete tree;

    logging::log(LEVEL_INFO, "Packing success.");

    return {objs, length};
}

void Node::setDimensions (int _width, int _height, int _offsetX, int _offsetY) {
    width = _width;
    height = _height;
    offsetX = _offsetX;
    offsetY = _offsetY;
}

bool Node::insert (atlas_internal::InternalAtlasImage& img, int padding) {
    // recursively uses a depth-first search for a space which is bigger than the image
    // returns true if a space can be found, false otherwise
    // see http://blackpawn.com/texts/lightmaps/ for algorithm, which has been very slightly tweaked
    if (index != -1) {
        // image already present
        if (leftNode != nullptr && rightNode != nullptr) {
            // should always get here
            return leftNode->insert(img, padding) || rightNode->insert(img, padding);
        } else {
            // just in case
            return false;
        }
    }
    if (width >= img.xSize + padding && height >= img.ySize) {
        // image can be inserted
        if (img.xSize > img.ySize) {
            // split on x axis first
            leftNode = new Node();
            leftNode->setDimensions(width - img.xSize - padding, height, offsetX + img.xSize + padding, offsetY);

            // then split on y
            rightNode = new Node();
            rightNode->setDimensions(img.xSize, height - img.ySize - padding,
                                     offsetX, offsetY + img.ySize + padding);
        } else {
            // then split on y
            rightNode = new Node();
            rightNode->setDimensions(width, height - img.ySize - padding,
                                     offsetX, offsetY + img.ySize + padding);
            // split on x axis first
            leftNode = new Node();
            leftNode->setDimensions(width - img.xSize - padding, img.ySize, offsetX + img.xSize + padding, offsetY);
        }
        width = img.xSize;
        height = img.ySize;
        index = img.index;
        return true;
    } else {
        return false;
    }
}

void Node::destroy () {
    if (leftNode != nullptr) {
        leftNode->destroy();
        delete leftNode;
        leftNode = nullptr;
    }
    if (rightNode != nullptr) {
        rightNode->destroy();
        delete rightNode;
        rightNode = nullptr;
    }
    index = -1;
}

void Node::walk (std::back_insert_iterator<std::vector<Node*>> out) {
    if (this->index != -1) {
        *out = this;
        out++;
    }
    if (leftNode != nullptr) {
        leftNode->walk(out);
    }
    if (rightNode != nullptr) {
        rightNode->walk(out);
    }
    //return out;
}

static void printNode (Node* node, FILE* file) {
    fprintf(file, "{x: %d y: %d width: %d height: %d}\n", node->offsetX, node->offsetY, node->width, node->height);
}
static void printTreeHelp (Node* root, int height, bool right, FILE* file) {
    if (root == nullptr) {
        return;
    }
    if (height > 0) {
        for (int i = 0; i < height; i++) {
            if (i > 0) {
                fprintf(file, "   ");
            }
            fprintf(file, "|");
        }
        fprintf(file, "\n");
        for (int i = 0; i < height - 1; i++) {
            if (i > 0) {
                fprintf(file, "   ");
            }
            fprintf(file, "|");
        }
        if (height > 1) {
            fprintf(file, "   ");
        }
        if (right) {
            fprintf(file, "└");
        } else {
            fprintf(file, "├");
        }
        fprintf(file, "---");
    }
    printNode(root, file);
    printTreeHelp(root->leftNode, height + 1, false, file);
    printTreeHelp(root->rightNode, height + 1, true, file);
}

static void printTree (Node* root) {
    FILE* file = fopen("treeprint.txt", "a");
    printTreeHelp(root, 0, false, file);
    fprintf(file, "done\n");
    fclose(file);
}