#include "openal_system.h"

#include "audio/detail/loggers.h"
#include "logging/logging.h"

static phenyl::Logger LOGGER{"AL_SYSTEM", phenyl::audio::detail::AUDIO_LOGGER};

phenyl::audio::OpenALSystem::OpenALSystem () {
    PHENYL_LOGD(LOGGER, "Initialising OpenAL system");

    PHENYL_TRACE(LOGGER, "Opening device");
    m_device = alcOpenDevice(nullptr);
    if (!m_device) {
        PHENYL_LOGE(LOGGER, "Failed to open OpenAL device!");
        return;
    }
    alGetError(); // ignored

    PHENYL_TRACE(LOGGER, "Creating context");
    m_context = alcCreateContext(m_device, nullptr);
    if (!m_context) {
        auto err = alcGetError(m_device);

        PHENYL_LOGE(LOGGER, "Failed to create OpenAL context: {}!", ALcStrError(err));
        alcCloseDevice(m_device);
        m_device = nullptr;
        return;
    }

    PHENYL_TRACE(LOGGER, "Making context current");
    if (!alcMakeContextCurrent(m_context)) {
        auto err = alcGetError(m_device);

        PHENYL_LOGE(LOGGER, "Failed to make OpenAL context current: {}!", ALcStrError(err));

        alcDestroyContext(m_context);
        alcCloseDevice(m_device);
        m_context = nullptr;
        m_device = nullptr;
        return;
    }
}

phenyl::audio::OpenALSystem::~OpenALSystem () {
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(m_context);
    alcCloseDevice(m_device);
}

phenyl::audio::OpenALSystem::OpenALSystem (phenyl::audio::OpenALSystem&& other) noexcept : m_device{other.m_device}, m_context{other.m_context} {
    other.m_device = nullptr;
    other.m_context = nullptr;
}

phenyl::audio::OpenALSystem& phenyl::audio::OpenALSystem::operator= (phenyl::audio::OpenALSystem&& other) noexcept {
    if (m_context && alcGetCurrentContext() == m_context) {
        alcMakeContextCurrent(nullptr);
    }

    alcDestroyContext(m_context);
    alcCloseDevice(m_device);

    m_context = other.m_context;
    alcMakeContextCurrent(m_context);
    m_device = other.m_device;

    other.m_context = nullptr;
    other.m_device = nullptr;

    return *this;
}

std::size_t phenyl::audio::OpenALSystem::makeSource () {
    PHENYL_TRACE(LOGGER, "Making source");
    OpenALSource source{};
    if (!source) {
        PHENYL_LOGI(LOGGER, "Failed to make source");
        return 0;
    }

    auto sourceId = m_sources.emplace(std::move(source));

    PHENYL_TRACE(LOGGER, "Initialised source with sourceId={}", sourceId + 1);
    return sourceId + 1;
}
std::size_t phenyl::audio::OpenALSystem::makeWAVSample (const phenyl::audio::WAVFile& wavFile) {
    PHENYL_TRACE(LOGGER, "Making sample");
    OpenALBuffer buffer{wavFile};
    if (!buffer) {
        PHENYL_LOGI(LOGGER, "Failed to make buffer");
        return 0;
    }

    auto bufId = m_buffers.emplace(std::move(buffer));

    PHENYL_TRACE(LOGGER, "Initialised buffer with bufId={}", bufId + 1);
    return bufId + 1;
}


void phenyl::audio::OpenALSystem::destroySource (std::size_t id) {
    PHENYL_ASSERT_MSG(id, "Attempted to destroy empty source!");
    PHENYL_ASSERT_MSG(m_sources.present(id - 1), "Attempted to destroy invalid source {}", id);

    m_sources.remove(id - 1);
}

void phenyl::audio::OpenALSystem::destroySample (std::size_t id) {
    PHENYL_ASSERT_MSG(id, "Attempted to destroy empty buffer!");
    PHENYL_ASSERT_MSG(m_buffers.present(id - 1), "Attempted to destroy invalid buffer {}", id);

    m_buffers.remove(id - 1);
}

void phenyl::audio::OpenALSystem::playSample (std::size_t sourceId, std::size_t sampleId) {
    PHENYL_ASSERT_MSG(sourceId, "Attempted to play to empty source!");
    PHENYL_ASSERT_MSG(m_sources.present(sourceId - 1), "Attempted to play to invalid source {}", sourceId);

    PHENYL_ASSERT_MSG(sampleId, "Attempted to play empty sample!");
    PHENYL_ASSERT_MSG(m_buffers.present(sampleId - 1), "Attempted to play invalid sample {}", sampleId);

    auto& source = m_sources.at(sourceId - 1);
    auto& buffer = m_buffers.at(sampleId - 1);

    source.playBuffer(buffer);
}

float phenyl::audio::OpenALSystem::getSourceGain (std::size_t id) const {
    PHENYL_ASSERT_MSG(id, "Attempted to get gain of empty source!");
    PHENYL_ASSERT_MSG(m_sources.present(id - 1), "Attempted to get gain of invalid source {}", id);

    return m_sources.at(id - 1).getGain();
}

void phenyl::audio::OpenALSystem::setSourceGain (std::size_t id, float gain) {
    PHENYL_ASSERT_MSG(id, "Attempted to set gain of empty source!");
    PHENYL_ASSERT_MSG(m_sources.present(id - 1), "Attempted to set gain of invalid source {}", id);

    m_sources.at(id - 1).setGain(gain);
}

float phenyl::audio::OpenALSystem::getSampleDuration (std::size_t id) const {
    PHENYL_ASSERT_MSG(id, "Attempted to get sample duration of empty sample!");
    PHENYL_ASSERT_MSG(m_buffers.present(id - 1), "Attempted to get sample duration of invalid sample {}", id);

    return m_buffers[id - 1].duration();
}

void phenyl::audio::OpenALSystem::stopSource (std::size_t id) {
    PHENYL_ASSERT_MSG(id, "Attempted to stop empty source!");
    PHENYL_ASSERT_MSG(m_sources.present(id - 1), "Attempted to stop invalid source {}", id);

    m_sources[id - 1].stop();
}

bool phenyl::audio::OpenALSystem::isSourceStopped (std::size_t id) {
    PHENYL_ASSERT_MSG(id, "Attempted to access empty source!");
    PHENYL_ASSERT_MSG(m_sources.present(id - 1), "Attempted to access invalid source {}", id);

    return m_sources[id - 1].stopped();
}
