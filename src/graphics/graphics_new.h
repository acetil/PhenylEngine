#include<stdlib.h>

#include <string>
#include <utility>
#include <vector>
#include <utility>
#include <unordered_map>
#include <optional>
#include <logging/logging.h>
#include "textures/texture_atlas.h"
#ifndef GRAPHICS_NEW_H
#define GRAPHICS_NEW_H
namespace graphics {
    class FrameBuffer {
    public:
        virtual void bind () = 0;
    };
    class ShaderProgram {

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
    };
    class RenderLayer;
    class Camera {

    };
    class GraphicsData {
    public:

    };
    /*class RenderLayer {
    private:
        std::string name;
        ShaderProgram* program;
    public:
        RenderLayer (std::string name, ShaderProgram* program, TextureAtlas* atlas, int maxModels);
        std::string getName ();
        int getDataId (std::string dataName);
        void render ();
        int getUniformId (std::string uniformName);
        template<typename T>
        void applyUniform (int uniformId, T value);
        template<typename T>
        T* getDataPtr (int dataId, int modelNum);
        int getNumModels ();
        int addModel (int modelId);

    };*/
    struct BufferInfo {
        int numBuffers;
        std::vector<int> sizes;
        std::vector<std::pair<int,int>> elementSizes; // pair is typeSize, elementSize
        std::vector<bool> isStatic;
        BufferInfo (int numBuffers, std::vector<std::pair<int,int>> elementSizes, std::vector<int> sizes,std::vector<bool> isStatic) {
            this->numBuffers = numBuffers;
            this->elementSizes = std::move(elementSizes);
            this->isStatic = std::move(isStatic);
            this->sizes = std::move(sizes);
        }
        BufferInfo () {
            this->numBuffers = 0;
            this->elementSizes = std::vector<std::pair<int,int>>();
            this->sizes = std::vector<int>();
            this->isStatic = std::vector<bool>();
        }
        [[nodiscard]]
        bool isEmpty () const {
            return numBuffers == 0;
        }
    };

    class Buffer {
    private:
        bool ownsMemory;
        void* memory;
        int elementSize;
        int numElements;
        int maxNumElements;
        bool isStatic;
    public:
        Buffer (int maxNumElements, int elementSize, bool isStatic);
        ~Buffer ();
        Buffer (const Buffer& copy); // copy constructor, does NOT move memory ownership
        Buffer (Buffer&& move) noexcept ; // move constructor, DOES move memory ownership
        template <typename T>
        void pushData (T* data, int num) {
            if (sizeof(T) != elementSize) {
                logging::logf(LEVEL_WARNING, "Attempted to push data with element size %d, but buffer element size is %d!",
                        sizeof(T), elementSize);
                return;
            }
            if (numElements >= maxNumElements) {
                logging::log(LEVEL_WARNING, "Attempted to push an element to buffer when buffer was at capacity!");
            }
            memcpy((T*)memory + numElements, data, num * sizeof(T));
            (T*)memory += num;
        }

    };
    class Renderer {
    public:
        virtual double getCurrentTime () = 0;
        virtual bool shouldClose () = 0;
        virtual void pollEvents () = 0;
        virtual void clearWindow () = 0;
        virtual FrameBuffer* getWindowBuffer () = 0;
    };
    class RenderLayer {
    public:
        virtual std::string getName () = 0;
        virtual int getPriority () = 0;
        virtual bool isActive () = 0;
        virtual BufferInfo getBufferInfo () = 0;
        virtual void addBuffer (Buffer buf) = 0;
        virtual void gatherData () = 0;
        virtual void preRender (Renderer* renderer) = 0;
        virtual void getUniformId (std::string uniformName) = 0;
        virtual void applyUniform (int uniformId, void* data) = 0;
        virtual void applyCamera (Camera camera) = 0;
        virtual void render (Renderer* renderer, FrameBuffer* frameBuf) = 0;
    };



    class GraphicsNew {
    private:
        Renderer* renderer;

        double lastTime;
        double deltaTime;

        RenderLayer* renderLayer;

        std::unordered_map<std::string, TextureAtlas> atlases;


        Camera camera;

    public:
        explicit GraphicsNew (Renderer* renderer);
        bool shouldClose ();
        void pollEvents ();
        void render ();
        //void addShader (std::string name, ShaderProgram* program);
        void initTextureAtlas (const std::string& atlasName, const std::vector<Model>& images);
        std::optional<TextureAtlas> getTextureAtlas (const std::string& atlas);
        void sync (int fps);
        double getDeltaTime () const;
        //int createLayer (RenderLayer* layer);
        //RenderLayer* getLayer (std::string name);
        //virtual RenderLayer* getLayer (int layer);
        Camera& getCamera ();
    };
}

#endif //GRAPHICS_NEW_H
