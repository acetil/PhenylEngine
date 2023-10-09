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
    cleanup();
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

phenyl::audio::AudioSource phenyl::audio::OpenALSystem::createSource () {
    OpenALSource source{};
    if (!source) {
        return {};
    }

    auto sourceId = sources.emplace(std::move(source));

    return makeSource(sourceId + 1);
}

phenyl::audio::AudioSample phenyl::audio::OpenALSystem::makeWAVSample (const phenyl::audio::WAVFile& wavFile) {
    OpenALBuffer buffer{wavFile};
    if (!buffer) {
        return {};
    }

    auto bufId = buffers.emplace(std::move(buffer));

    return makeSample(bufId + 1);
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
