#include "logging/logging.h"

#include "openal_source.h"
#include "openal_buffer.h"

using namespace phenyl::audio;

phenyl::audio::OpenALSource::OpenALSource () : valid{true}, sourceId{0} {
    alGetError();
    alGenSources(1, &sourceId);

    ALenum err;
    if ((err = alGetError())) {
        logging::log(LEVEL_ERROR, "OpenAL source creation failed: {}", ALStrError(err));
        valid = false;
    }
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
    alGetError();
    alSourcei(sourceId, AL_BUFFER, (ALint)buffer.id());
    ALenum err;
    if ((err = alGetError())) {
        logging::log(LEVEL_ERROR, "Failed to play sample {} to source {}: {}", buffer.id(), sourceId, ALStrError(err));
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
        logging::log(LEVEL_ERROR, "Failed to get gain of sample {}: {}", sourceId, ALStrError(err));
        return 0.0f;
    }

    return gain;
}

void OpenALSource::setGain (float gain) {
    alGetError();

    alSourcef(sourceId, AL_GAIN, gain);

    ALenum err;
    if ((err = alGetError())) {
        logging::log(LEVEL_ERROR, "Failed to set gain of sample {} to {}: {}", sourceId, gain, ALStrError(err));
    }
}
