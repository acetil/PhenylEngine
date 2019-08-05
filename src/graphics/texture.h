#include <string>
#ifndef TEXTURE_H
    #define TEXTURE_H
    namespace graphics {
        class Texture {
            private:
                float* texUvs;
                std::string name;
            public:
                float* getTexUvs ();
                void setTexUvs (int uPixel, int vPixel, int xSize, int ySize, int atlasXSize, int atlasYSize);
                std::string getName ();
                Texture (std::string name);
                ~Texture();
        };
    }
#endif