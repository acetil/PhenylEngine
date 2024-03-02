#include "logging/logging.h"

#include "openal_source.h"
#include "openal_buffer.h"

using namespace phenyl::audio;

static phenyl::Logger LOGGER{"AL_SOURCE"};

phenyl::audio::OpenALSource::OpenALSource () : sourceId{0}, valid{true} {
    alGetError();
    alGenSources(1, &sourceId);

    ALenum err;
    if ((err = alGetError())) {
        PHENYL_LOGE(LOGGER, "OpenAL source creation failed: {}", ALStrError(err));
        valid = false;
    }

    PHENYL_TRACE_IF(valid, LOGGER, "Created source with id={}", sourceId);
}

phenyl::audio::OpenALSource::~OpenALSource () {
    if (valid) {
        alDeleteSources(1, &sourceId);
    }
}

OpenALSource::OpenALSource (OpenALSource&& other) noexcept : sourceId{other.sourceId}, valid{other.valid} {
    other.sourceId = 0;
    other.valid = false;
}

OpenALSource& OpenALSource::operator= (OpenALSource&& other) noexcept {
    if (valid) {
        alDeleteSources(1, &sourceId);
    }

    sourceId = other.sourceId;
    valid = other.valid;

    other.sourceId = 0;
    other.valid = false;

    return *this;
}

void OpenALSource::playBuffer (const OpenALBuffer& buffer) {
    PHENYL_LOGD(LOGGER, "Source id={} playing buffer id={}", sourceId, buffer.id());

    alGetError();
    alSourcei(sourceId, AL_BUFFER, (ALint)buffer.id());
    ALenum err;
    if ((err = alGetError())) {
        PHENYL_LOGE(LOGGER, "Failed to play sample {} to source {}: {}", buffer.id(), sourceId, ALStrError(err));
        return;
    }
    alSourcePlay(sourceId);
}

float OpenALSource::getGain () const {
    alGetError();

    float gain;
    alGetSourcef(sourceId, AL_GAIN, &gain);

    ALenum err;
    if ((err = alGetError())) {
        PHENYL_LOGE(LOGGER, "Failed to get gain of source {}: {}", sourceId, ALStrError(err));
        return 0.0f;
    }

    return gain;
}

void OpenALSource::setGain (float gain) {
    alGetError();

    alSourcef(sourceId, AL_GAIN, gain);

    ALenum err;
    if ((err = alGetError())) {
        PHENYL_LOGE(LOGGER, "Failed to set gain of source {} to {}: {}", sourceId, gain, ALStrError(err));
    }
}

void OpenALSource::stop () {
    alGetError();
    alSourceStop(sourceId);

    ALenum err;
    if ((err = alGetError())) {
        PHENYL_LOGE(LOGGER, "Failed to stop source {}: {}", sourceId, ALStrError(err));
    }
}

bool OpenALSource::stopped () const {
    ALint status;
    alGetSourcei(sourceId, AL_SOURCE_STATE, &status);

    return status == AL_STOPPED;
}
