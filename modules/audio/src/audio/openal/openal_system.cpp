#include "openal_system.h"

#include "logging/logging.h"

phenyl::audio::OpenALSystem::OpenALSystem () {
    device = alcOpenDevice(nullptr);
    if (!device) {
        logging::log(LEVEL_ERROR, "Failed to open OpenAL device!");
        return;
    }
    alGetError(); // ignored
    context = alcCreateContext(device, nullptr);
    if (!context) {
        auto err = alcGetError(device);

        logging::log(LEVEL_ERROR, "Failed to create OpenAL context: {}!", ALcStrError(err));
        alcCloseDevice(device);
        device = nullptr;
        return;
    }

    if (!alcMakeContextCurrent(context)) {
        auto err = alcGetError(device);

        logging::log(LEVEL_ERROR, "Failed to make OpenAL context current: {}!", ALcStrError(err));

        alcDestroyContext(context);
        alcCloseDevice(device);
        context = nullptr;
        device = nullptr;
        return;
    }
}

phenyl::audio::OpenALSystem::~OpenALSystem () {
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(context);
    alcCloseDevice(device);
}

phenyl::audio::OpenALSystem::OpenALSystem (phenyl::audio::OpenALSystem&& other) noexcept : device{other.device}, context{other.context} {
    other.device = nullptr;
    other.context = nullptr;
}

phenyl::audio::OpenALSystem& phenyl::audio::OpenALSystem::operator= (phenyl::audio::OpenALSystem&& other) noexcept {
    if (context && alcGetCurrentContext() == context) {
        alcMakeContextCurrent(nullptr);
    }

    alcDestroyContext(context);
    alcCloseDevice(device);

    context = other.context;
    alcMakeContextCurrent(context);
    device = other.device;

    other.context = nullptr;
    other.device = nullptr;

    return *this;
}

std::size_t phenyl::audio::OpenALSystem::makeSource () {
    OpenALSource source{};
    if (!source) {
        return 0;
    }

    auto sourceId = sources.emplace(std::move(source));

    return sourceId + 1;
}
std::size_t phenyl::audio::OpenALSystem::makeWAVSample (const phenyl::audio::WAVFile& wavFile) {
    OpenALBuffer buffer{wavFile};
    if (!buffer) {
        return 0;
    }

    auto bufId = buffers.emplace(std::move(buffer));

    return bufId + 1;
}


void phenyl::audio::OpenALSystem::destroySource (std::size_t id) {
    if (!id) {
        logging::log(LEVEL_ERROR, "Attempted to destroy empty source!");
    } else if (!sources.present(id - 1)) {
        logging::log(LEVEL_ERROR, "Attempted to destroy invalid source {}!", id);
    }

    sources.remove(id - 1);
}

void phenyl::audio::OpenALSystem::destroySample (std::size_t id) {
    if (!id) {
        logging::log(LEVEL_ERROR, "Attempted to destroy empty sample!");
    } else if (!buffers.present(id - 1)) {
        logging::log(LEVEL_ERROR, "Attempted to destroy invalid sample {}!", id);
    }

    buffers.remove(id - 1);
}

void phenyl::audio::OpenALSystem::playSample (std::size_t sourceId, std::size_t sampleId) {
    if (!sourceId) {
        logging::log(LEVEL_ERROR, "Attempted to play to empty source!");
        return;
    } else if (!sources.present(sourceId - 1)) {
        logging::log(LEVEL_ERROR, "Attempted to play to invalid source {}!", sourceId);
        return;
    }

    if (!sampleId) {
        logging::log(LEVEL_ERROR, "Attempted to play empty sample!");
        return;
    } else if (!buffers.present(sampleId - 1)) {
        logging::log(LEVEL_ERROR, "Attempted to play invalid sample {}!", sampleId);
        return;
    }

    auto& source = sources.at(sourceId - 1);
    auto& buffer = buffers.at(sampleId - 1);

    source.playBuffer(buffer);
}

float phenyl::audio::OpenALSystem::getSourceGain (std::size_t id) const {
    if (!id) {
        logging::log(LEVEL_ERROR, "Attempted to get gain of empty source!");
        return 0;
    } else if (!sources.present(id - 1)) {
        logging::log(LEVEL_ERROR, "Attempted to get gain of invalid source {}!", id);
        return 0;
    }

    return sources.at(id - 1).getGain();
}

void phenyl::audio::OpenALSystem::setSourceGain (std::size_t id, float gain) {
    if (!id) {
        logging::log(LEVEL_ERROR, "Attempted to set gain of empty source!");
        return;
    } else if (!sources.present(id - 1)) {
        logging::log(LEVEL_ERROR, "Attempted to set gain of invalid source {}!", id);
        return;
    }

    sources.at(id - 1).setGain(gain);
}

float phenyl::audio::OpenALSystem::getSampleDuration (std::size_t id) const {
    if (!id) {
        logging::log(LEVEL_ERROR, "Attempted to get sample duration of empty sample!");
        return 0.0f;
    } else if (!buffers.present(id - 1)) {
        logging::log(LEVEL_ERROR, "Attempted to get sample duration of invalid sample {}!", id);
        return 0.0f;
    }

    return buffers[id - 1].duration();
}

void phenyl::audio::OpenALSystem::stopSource (std::size_t id) {
    if (!id) {
        logging::log(LEVEL_ERROR, "Attempted to stop empty source!");
        return;
    } else if (!sources.present(id - 1)) {
        logging::log(LEVEL_ERROR, "Attempted to stop invalid source {}!", id);
        return;
    }

    sources[id - 1].stop();
}

bool phenyl::audio::OpenALSystem::isSourceStopped (std::size_t id) {
    if (!id) {
        logging::log(LEVEL_ERROR, "Attempted to stop empty source!");
        return false;
    } else if (!sources.present(id - 1)) {
        logging::log(LEVEL_ERROR, "Attempted to stop invalid source {}!", id);
        return false;
    }

    return sources[id - 1].stopped();
}
