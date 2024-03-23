#pragma once

#include "logging/properties.h"
#include "graphics/graphics_properties.h"

namespace phenyl {
    class PhenylEngine;

    namespace engine {
        class Engine;
    }

    class ApplicationProperties {
    private:
        graphics::GraphicsProperties graphicsProperties;
        logging::LoggingProperties loggingProperties;

        friend class engine::Engine;
        friend class PhenylEngine;
    public:
        ApplicationProperties () = default;

        ApplicationProperties& withResolution (const int width, const int height) {
            graphicsProperties.withWindowSize(width, height);

            return *this;
        }

        ApplicationProperties& withWindowTitle (std::string title) {
            graphicsProperties.withWindowTitle(std::move(title));

            return *this;
        }

        ApplicationProperties& withVsync (const bool vsync) {
            graphicsProperties.withVsync(vsync);

            return *this;
        }

        ApplicationProperties& withLogFile (std::string logFile) {
            loggingProperties.withLogFile(std::move(logFile));

            return *this;
        }

        ApplicationProperties& withLogLevel (const std::string& logger, const int level) {
            loggingProperties.withLogLevel(logger, level);

            return *this;
        }

        ApplicationProperties& withRootLogLevel (const int level) {
            loggingProperties.withRootLogLevel(level);

            return *this;
        }
    };
}