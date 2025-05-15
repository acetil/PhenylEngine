#pragma once

#include "../../modules/graphics/backends/api/include/graphics/graphics_properties.h"
#include "logging/properties.h"

namespace phenyl {
class PhenylEngine;

namespace engine {
    class Engine;
}

class ApplicationProperties {
private:
    graphics::GraphicsProperties m_graphics;
    logging::LoggingProperties m_logging;

    friend class engine::Engine;
    friend class PhenylEngine;

public:
    ApplicationProperties () = default;

    ApplicationProperties& withResolution (const int width, const int height) {
        m_graphics.withWindowSize(width, height);

        return *this;
    }

    ApplicationProperties& withWindowTitle (std::string title) {
        m_graphics.withWindowTitle(std::move(title));

        return *this;
    }

    ApplicationProperties& withVsync (const bool vsync) {
        m_graphics.withVsync(vsync);

        return *this;
    }

    ApplicationProperties& withLogFile (std::string logFile) {
        m_logging.withLogFile(std::move(logFile));

        return *this;
    }

    ApplicationProperties& withLogLevel (const std::string& logger, const int level) {
        m_logging.withLogLevel(logger, level);

        return *this;
    }

    ApplicationProperties& withRootLogLevel (const int level) {
        m_logging.withRootLogLevel(level);

        return *this;
    }

    // TODO: nicer design
    const logging::LoggingProperties& logging () const noexcept {
        return m_logging;
    }
};
} // namespace phenyl
