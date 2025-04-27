#include "logging/logging.h"

#include "audio/detail/loggers.h"
#include "openal_source.h"
#include "openal_buffer.h"

using namespace phenyl::audio;

static phenyl::Logger LOGGER{"AL_SOURCE", detail::AUDIO_LOGGER};

phenyl::audio::OpenALSource::OpenALSource () : m_id{0}, m_valid{true} {
    alGetError();
    alGenSources(1, &m_id);

    ALenum err;
    if ((err = alGetError())) {
        PHENYL_LOGE(LOGGER, "OpenAL source creation failed: {}", ALStrError(err));
        m_valid = false;
    }

    PHENYL_TRACE_IF(m_valid, LOGGER, "Created source with id={}", m_id);
}

phenyl::audio::OpenALSource::~OpenALSource () {
    if (m_valid) {
        alDeleteSources(1, &m_id);
    }
}

OpenALSource::OpenALSource (OpenALSource&& other) noexcept : m_id{other.m_id}, m_valid{other.m_valid} {
    other.m_id = 0;
    other.m_valid = false;
}

OpenALSource& OpenALSource::operator= (OpenALSource&& other) noexcept {
    if (m_valid) {
        alDeleteSources(1, &m_id);
    }

    m_id = other.m_id;
    m_valid = other.m_valid;

    other.m_id = 0;
    other.m_valid = false;

    return *this;
}

void OpenALSource::playBuffer (const OpenALBuffer& buffer) {
    PHENYL_TRACE(LOGGER, "Source id={} playing buffer id={}", m_id, buffer.id());

    alGetError();
    alSourcei(m_id, AL_BUFFER, (ALint)buffer.id());
    ALenum err;
    if ((err = alGetError())) {
        PHENYL_LOGE(LOGGER, "Failed to play sample {} to source {}: {}", buffer.id(), m_id, ALStrError(err));
        return;
    }
    alSourcePlay(m_id);
}

float OpenALSource::getGain () const {
    alGetError();

    float gain;
    alGetSourcef(m_id, AL_GAIN, &gain);

    ALenum err;
    if ((err = alGetError())) {
        PHENYL_LOGE(LOGGER, "Failed to get gain of source {}: {}", m_id, ALStrError(err));
        return 0.0f;
    }

    return gain;
}

void OpenALSource::setGain (float gain) {
    alGetError();

    alSourcef(m_id, AL_GAIN, gain);

    ALenum err;
    if ((err = alGetError())) {
        PHENYL_LOGE(LOGGER, "Failed to set gain of source {} to {}: {}", m_id, gain, ALStrError(err));
    }
}

void OpenALSource::stop () {
    alGetError();
    alSourceStop(m_id);

    ALenum err;
    if ((err = alGetError())) {
        PHENYL_LOGE(LOGGER, "Failed to stop source {}: {}", m_id, ALStrError(err));
    }
}

bool OpenALSource::stopped () const {
    ALint status;
    alGetSourcei(m_id, AL_SOURCE_STATE, &status);

    return status == AL_STOPPED;
}
