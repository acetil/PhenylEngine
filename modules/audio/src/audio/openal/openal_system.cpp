#include "openal_system.h"

#include "logging/logging.h"

static phenyl::Logger LOGGER{"AL_SYSTEM"};

phenyl::audio::OpenALSystem::OpenALSystem () {
    PHENYL_LOGD(LOGGER, "Initialising OpenAL system");

    PHENYL_LOGT(LOGGER, "Opening device");
    device = alcOpenDevice(nullptr);
    if (!device) {
        PHENYL_LOGE(LOGGER, "Failed to open OpenAL device!");
        return;
    }
    alGetError(); // ignored

    PHENYL_LOGT(LOGGER, "Creating context");
    context = alcCreateContext(device, nullptr);
    if (!context) {
        auto err = alcGetError(device);

        PHENYL_LOGE(LOGGER, "Failed to create OpenAL context: {}!", ALcStrError(err));
        alcCloseDevice(device);
        device = nullptr;
        return;
    }

    PHENYL_LOGT(LOGGER, "Making context current");
    if (!alcMakeContextCurrent(context)) {
        auto err = alcGetError(device);

        PHENYL_LOGE(LOGGER, "Failed to make OpenAL context current: {}!", ALcStrError(err));

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
    PHENYL_LOGT(LOGGER, "Making source");
    OpenALSource source{};
    if (!source) {
        PHENYL_LOGI(LOGGER, "Failed to make source");
        return 0;
    }

    auto sourceId = sources.emplace(std::move(source));

    PHENYL_LOGT(LOGGER, "Initialised source with sourceId={}", sourceId + 1);
    return sourceId + 1;
}
std::size_t phenyl::audio::OpenALSystem::makeWAVSample (const phenyl::audio::WAVFile& wavFile) {
    PHENYL_LOGT(LOGGER, "Making sample");
    OpenALBuffer buffer{wavFile};
    if (!buffer) {
        PHENYL_LOGI(LOGGER, "Failed to make buffer");
        return 0;
    }

    auto bufId = buffers.emplace(std::move(buffer));

    PHENYL_LOGT(LOGGER, "Initialised buffer with bufId={}", bufId + 1);
    return bufId + 1;
}


void phenyl::audio::OpenALSystem::destroySource (std::size_t id) {
    PHENYL_ASSERT_MSG(id, "Attempted to destroy empty source!");
    PHENYL_ASSERT_MSG(sources.present(id - 1), "Attempted to destroy invalid source {}", id);

    sources.remove(id - 1);
}

void phenyl::audio::OpenALSystem::destroySample (std::size_t id) {
    PHENYL_ASSERT_MSG(id, "Attempted to destroy empty buffer!");
    PHENYL_ASSERT_MSG(buffers.present(id - 1), "Attempted to destroy invalid buffer {}", id);

    buffers.remove(id - 1);
}

void phenyl::audio::OpenALSystem::playSample (std::size_t sourceId, std::size_t sampleId) {
    PHENYL_ASSERT_MSG(sourceId, "Attempted to play to empty source!");
    PHENYL_ASSERT_MSG(sources.present(sourceId - 1), "Attempted to play to invalid source {}", sourceId);

    PHENYL_ASSERT_MSG(sampleId, "Attempted to play empty sample!");
    PHENYL_ASSERT_MSG(buffers.present(sampleId - 1), "Attempted to play invalid sample {}", sampleId);

    auto& source = sources.at(sourceId - 1);
    auto& buffer = buffers.at(sampleId - 1);

    source.playBuffer(buffer);
}

float phenyl::audio::OpenALSystem::getSourceGain (std::size_t id) const {
    PHENYL_ASSERT_MSG(id, "Attempted to get gain of empty source!");
    PHENYL_ASSERT_MSG(sources.present(id - 1), "Attempted to get gain of invalid source {}", id);

    return sources.at(id - 1).getGain();
}

void phenyl::audio::OpenALSystem::setSourceGain (std::size_t id, float gain) {
    PHENYL_ASSERT_MSG(id, "Attempted to set gain of empty source!");
    PHENYL_ASSERT_MSG(sources.present(id - 1), "Attempted to set gain of invalid source {}", id);

    sources.at(id - 1).setGain(gain);
}

float phenyl::audio::OpenALSystem::getSampleDuration (std::size_t id) const {
    PHENYL_ASSERT_MSG(id, "Attempted to get sample duration of empty sample!");
    PHENYL_ASSERT_MSG(buffers.present(id - 1), "Attempted to get sample duration of invalid sample {}", id);

    return buffers[id - 1].duration();
}

void phenyl::audio::OpenALSystem::stopSource (std::size_t id) {
    PHENYL_ASSERT_MSG(id, "Attempted to stop empty source!");
    PHENYL_ASSERT_MSG(sources.present(id - 1), "Attempted to stop invalid source {}", id);

    sources[id - 1].stop();
}

bool phenyl::audio::OpenALSystem::isSourceStopped (std::size_t id) {
    PHENYL_ASSERT_MSG(id, "Attempted to access empty source!");
    PHENYL_ASSERT_MSG(sources.present(id - 1), "Attempted to access invalid source {}", id);

    return sources[id - 1].stopped();
}
