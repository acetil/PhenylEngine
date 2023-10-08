#include "openal_system.h"

#include "logging/logging.h"

phenyl::audio::OpenALSystem::OpenALSystem () {
    device = alcOpenDevice(nullptr);
    if (!device) {
        logging::log(LEVEL_ERROR, "Failed to open OpenAL device!");
    }
    alGetError(); // ignored
}

phenyl::audio::OpenALSystem::~OpenALSystem () {
    alcCloseDevice(device);
}
