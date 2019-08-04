#ifndef TEXTURE_H
    #define TEXTURE_H
    namespace graphics {
        class Texture {
            public:
                float* getTexUvs ();
                char* getName ();
        };
    }
#endif