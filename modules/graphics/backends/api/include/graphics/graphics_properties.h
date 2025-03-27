#pragma once

#include <string>

namespace phenyl::graphics {
    class GraphicsProperties {
    private:
        int windowWidth = 800;
        int windowHeight = 600;
        std::string windowTitle = "Phenyl Engine";
        bool vsync = false;
    public:
        GraphicsProperties () = default;

        GraphicsProperties& withWindowSize (int width, int height) {
            windowWidth = width;
            windowHeight = height;

            return *this;
        }

        GraphicsProperties& withWindowTitle (std::string title) {
            windowTitle = std::move(title);

            return *this;
        }

        GraphicsProperties& withVsync (bool doVsync) {
            vsync = doVsync;

            return *this;
        }

        [[nodiscard]] int getWindowWidth () const {
            return windowWidth;
        }

        [[nodiscard]] int getWindowHeight () const {
            return windowHeight;
        }

        [[nodiscard]] const std::string& getWindowTitle () const {
            return windowTitle;
        }

        bool getVsync () const {
            return vsync;
        }
    };
}