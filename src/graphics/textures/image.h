#include <string>
#ifndef IMAGE_H
    #define IMAGE_H
    namespace graphics {
        class Image {
            private:
                unsigned char* data;
                int width;
                int height;
                int n;
                std::string name;
            public:
                Image (const char* filename, std::string);
                ~Image ();
                unsigned char* getData ();
        };
    }
#endif