#include <stdio.h>
#include <string>
#include <utility>
#include <vector>
#include <utility>
#include <optional>

#include "graphics/shaders/shaders.h"
#include "textures/texture.h"
#include "textures/image.h"
#include "logging/logging.h"
#include "camera.h"
#ifndef GRAPHICS_NEW_INCLUDE_H
#define GRAPHICS_NEW_INCLUDE_H
namespace graphics {
    class FrameBuffer {
    public:
        virtual void bind () = 0;
    };

    struct TextureOffset {
        float x;
        float y;

        TextureOffset (float x, float y) {
            this->x = x;
            this->y = y;
        }
    };

    struct Model {
        std::string modelName;
        std::string texPath;
        std::vector<std::pair<TextureOffset, Image*>> textures;
        Model (std::string modelName, Image* image) {
            this->modelName = std::move(modelName);
            textures.emplace_back(TextureOffset(0.0f, 0.0f), image);
            texPath = "";
        }
    };

    class RenderLayer;
    class BufferNew;

    // TODO: roll into BufferNew
    struct GraphicsBufferIds {
        unsigned int vaoId;
        std::vector<unsigned int> vboIds;
        GraphicsBufferIds (): vaoId(0), vboIds({}) {};
        GraphicsBufferIds (unsigned int _vaoId, std::vector<unsigned int> _vboIds): vaoId(_vaoId), vboIds(std::move(_vboIds)) {};
    };

    class Renderer {
    public:
        virtual double getCurrentTime () = 0;

        virtual bool shouldClose () = 0;

        virtual void pollEvents () = 0;

        virtual void clearWindow () = 0;

        virtual FrameBuffer* getWindowBuffer () = 0;
        virtual std::optional<ShaderProgram*> getProgram (std::string program) = 0;
        virtual GraphicsBufferIds getBufferIds (int requestedBufs, int bufferSize) = 0;
        virtual void bufferData (GraphicsBufferIds& ids, BufferNew* buffers) = 0; // TODO: make more safe

        virtual void render (GraphicsBufferIds& ids, ShaderProgram* program, int numTriangles) = 0; // TODO: put rendering through frame buffer?

        virtual void finishRender () = 0;
    };

    struct BufferInfo {
        int numBuffers;
        std::vector<int> sizes;
        std::vector<std::pair<int, int>> elementSizes; // pair is typeSize, elementSize
        std::vector<bool> isStatic;

        BufferInfo (int numBuffers, std::vector<std::pair<int, int>> elementSizes, std::vector<int> sizes,
                    std::vector<bool> isStatic) {
            this->numBuffers = numBuffers;
            this->elementSizes = std::move(elementSizes);
            this->isStatic = std::move(isStatic);
            this->sizes = std::move(sizes);
        }

        BufferInfo () {
            this->numBuffers = 0;
        }

        [[nodiscard]]
        bool isEmpty () const {
            return numBuffers == 0;
        }

        BufferInfo operator+ (const BufferInfo& val) {
            if (val.isEmpty()) {
                return *this;
            }
            std::vector<std::pair<int,int>> eSize;
            std::vector<int> szs;
            std::vector<bool> stat;

            eSize.reserve(elementSizes.size() + val.elementSizes.size());
            szs.reserve(sizes.size() + val.sizes.size());
            stat.reserve(isStatic.size() + val.isStatic.size());

            eSize.insert(eSize.end(), elementSizes.begin(), elementSizes.end());
            eSize.insert(eSize.end(), val.elementSizes.begin(), val.elementSizes.end());
            szs.insert(szs.end(), sizes.begin(), sizes.end());
            szs.insert(szs.end(), val.sizes.begin(), val.sizes.end());
            stat.insert(stat.end(), isStatic.begin(), isStatic.end());
            stat.insert(stat.end(), val.isStatic.begin(), val.isStatic.end());

            return BufferInfo(numBuffers + val.numBuffers, eSize, szs, stat);
        }
        void operator+= (const BufferInfo& val) {
            if (val.isEmpty()) {
                return;
            }
            elementSizes.reserve(elementSizes.size() + val.elementSizes.size());
            elementSizes.insert(elementSizes.end(), val.elementSizes.begin(), val.elementSizes.end());

            sizes.reserve(sizes.size() + val.sizes.size());
            sizes.insert(sizes.end(), val.sizes.begin(), val.sizes.end());

            isStatic.reserve(isStatic.size() + val.isStatic.size());
            isStatic.insert(isStatic.end(), val.isStatic.begin(), val.isStatic.end());
            numBuffers += val.numBuffers;
        }
    };

    // TODO: use unique_ptr, use vao/vbo stuff
    class BufferNew {
    private:
        bool ownsMemory;
        void* memory;
        int elementSize;
        int numElements;
        int maxNumElements;
        bool isStatic;
    public:
        BufferNew () {
            ownsMemory = false;
            memory = nullptr;
            elementSize = 0;
            numElements = 0;
            maxNumElements = 0;
            isStatic = false;
        }
        BufferNew (int maxNumElements, int elementSize, bool isStatic);

        ~BufferNew ();

        BufferNew (const BufferNew &copy); // copy constructor, does NOT move memory ownership
        BufferNew (BufferNew &&move) noexcept; // move constructor, DOES move memory ownership
        BufferNew& operator= (BufferNew && move) noexcept;
        template<typename T>
        void pushData (T* data, int num) {
            if (sizeof(T) != elementSize) {
                logging::logf(LEVEL_WARNING,
                              "Attempted to push data with element size %d, but buffer element size is %d!",
                              sizeof(T), elementSize);
                return;
            }
            if (numElements >= maxNumElements) {
                logging::log(LEVEL_WARNING, "Attempted to push an element to buffer when buffer was at capacity!");
            }
            memcpy(((T*) memory) + numElements, data, num * sizeof(T));
            numElements += 2;
        }

        

        [[nodiscard]]
        bool isEmpty () const {
            return maxNumElements == 0;
        }
        [[nodiscard]]
        int currentSize () const {
            return elementSize * numElements;
        };
        void* getData () {
            return memory;
        }
        void clearData () {
            numElements = 0;
        }
    };

}
#endif //GRAPHICS_NEW_INCLUDE_H
