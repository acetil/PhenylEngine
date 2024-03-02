#include "openal_buffer.h"

using namespace phenyl::audio;

static phenyl::Logger LOGGER{"AL_BUF"};

OpenALBuffer::OpenALBuffer (const phenyl::audio::WAVFile& wavFile) {
    alGetError();
    alGenBuffers(1, &bufferId);

    PHENYL_TRACE(LOGGER, "Initialising buffer with id={}", bufferId);

    ALenum err;
    if ((err = alGetError())) {
        PHENYL_LOGE(LOGGER, "Failed to create OpenAL buffer: {}", ALStrError(err));
        return;
    }

    ALenum format;
    if (wavFile.getNumChannels() == 1 && wavFile.getBitDepth() == 8) {
        format = AL_FORMAT_MONO8;
    } else if (wavFile.getNumChannels() == 1 && wavFile.getBitDepth() == 16) {
        format = AL_FORMAT_MONO16;
    } else if (wavFile.getNumChannels() == 2 && wavFile.getBitDepth() == 8) {
        format = AL_FORMAT_STEREO8;
    } else if (wavFile.getNumChannels() == 2 && wavFile.getBitDepth() == 16) {
        format = AL_FORMAT_STEREO16;
    } else {
        PHENYL_LOGE(LOGGER, "Unsupported WAV file settings: channels={}, bit depth={}", wavFile.getNumChannels(), wavFile.getBitDepth());
        return;
    }

    alBufferData(bufferId, format, wavFile.getData(), (int)wavFile.getDataSize(), (int)wavFile.getSampleRate());
    if ((err = alGetError())) {
        PHENYL_LOGE(LOGGER, "Failed to buffer data to OpenAL buffer: {}", ALStrError(err));
        return;
    }
    valid = true;
}

OpenALBuffer::OpenALBuffer (OpenALBuffer&& other) noexcept : bufferId{other.bufferId}, valid{other.valid} {
    other.bufferId = 0;
    other.valid = false;
}

OpenALBuffer& OpenALBuffer::operator= (OpenALBuffer&& other) noexcept {
    if (valid) {
        alDeleteBuffers(1, &bufferId);
    }

    bufferId = other.bufferId;
    valid = other.valid;

    other.bufferId = 0;
    other.valid = false;
    return *this;
}

OpenALBuffer::~OpenALBuffer () {
    if (valid) {
        alDeleteBuffers(1, &bufferId);
    }
}

float OpenALBuffer::duration () const {
    if (!valid) {
        PHENYL_LOGD(LOGGER, "Attempted to get duration for invalid buffer");
        return 0.0f;
    }
    ALint frequency, channels, bits, size;
    alGetBufferi(bufferId, AL_FREQUENCY, &frequency);
    alGetBufferi(bufferId, AL_CHANNELS, &channels);
    alGetBufferi(bufferId, AL_BITS, &bits);
    alGetBufferi(bufferId, AL_SIZE, &size);

    PHENYL_TRACE(LOGGER, "Buffer id={} freq={}, channels={}, bits={}, size={}", bufferId, frequency, channels, bits, size);
    return ((float) size) / (float) (frequency * channels * bits / 8);
}
