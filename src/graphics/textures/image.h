#include <string>
#ifndef IMAGE_H
    #define IMAGE_H
    namespace graphics {
        class Image {
            private:
                unsigned char* data;
                int width{};
                int height{};
                int n{};
                std::string name;
                bool isSTB;
            public:
                Image (const char* filename, std::string name);
                Image (unsigned char* data, int width, int height, bool monochrome, const std::string& name);
                ~Image ();
                unsigned char* getData ();
                int getArea () const;
                int getWidth () const;
                int getHeight () const;
                std::string getName();
        };
    }
#endif