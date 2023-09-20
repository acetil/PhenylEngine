#pragma once

#include <string>
#include <memory>

#include "util/smart_help.h"

namespace phenyl::graphics {
    class Image : public util::SmartHelper<Image>{
        private:
            struct DataDeleter {
                bool isSTB;
                void operator() (std::byte* data) const noexcept;
            };

            //unsigned char* data;
            std::unique_ptr<std::byte[], DataDeleter> imageData{};
            int width{};
            int height{};
            int n{};
            std::string name{};
        public:
            Image (const char* filename, std::string name);
            Image (std::string name, int width, int height, int n);
            explicit Image (std::istream& file);
            Image (const Image& other) = delete;
            Image (Image&& image) noexcept;

            Image& operator= (const Image& other) = delete;
            Image& operator= (Image&& other) noexcept;
            ~Image ();

            explicit operator bool () const;

            std::byte* getData () const;
            int getArea () const;
            int getWidth () const;
            int getHeight () const;
            std::string getName();
    };
}
