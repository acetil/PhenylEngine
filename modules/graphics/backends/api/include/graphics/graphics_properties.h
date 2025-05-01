#pragma once

#include <string>

namespace phenyl::graphics {
    class GraphicsProperties {
    private:
        int m_windowWidth = 800;
        int m_windowHeight = 600;
        std::string m_windowTitle = "Phenyl Engine";
        bool m_vsync = false;
    public:
        GraphicsProperties () = default;

        GraphicsProperties& withWindowSize (int width, int height) {
            m_windowWidth = width;
            m_windowHeight = height;

            return *this;
        }

        GraphicsProperties& withWindowTitle (std::string title) {
            m_windowTitle = std::move(title);

            return *this;
        }

        GraphicsProperties& withVsync (bool doVsync) {
            m_vsync = doVsync;

            return *this;
        }

        [[nodiscard]] int getWindowWidth () const {
            return m_windowWidth;
        }

        [[nodiscard]] int getWindowHeight () const {
            return m_windowHeight;
        }

        [[nodiscard]] const std::string& getWindowTitle () const {
            return m_windowTitle;
        }

        bool getVsync () const {
            return m_vsync;
        }
    };
}