#include "openal_buffer.h"

#include "audio/detail/loggers.h"

using namespace phenyl::audio;

static phenyl::Logger LOGGER{"AL_BUF", detail::AUDIO_LOGGER};

OpenALBuffer::OpenALBuffer (const phenyl::audio::WAVFile& wavFile) {
    alGetError();
    alGenBuffers(1, &m_id);

    PHENYL_TRACE(LOGGER, "Initialising buffer with id={}", m_id);

    ALenum err;
    if ((err = alGetError())) {
        PHENYL_LOGE(LOGGER, "Failed to create OpenAL buffer: {}", ALStrError(err));
        return;
    }

    ALenum format;
    if (wavFile.numChannels() == 1 && wavFile.bitDepth() == 8) {
        format = AL_FORMAT_MONO8;
    } else if (wavFile.numChannels() == 1 && wavFile.bitDepth() == 16) {
        format = AL_FORMAT_MONO16;
    } else if (wavFile.numChannels() == 2 && wavFile.bitDepth() == 8) {
        format = AL_FORMAT_STEREO8;
    } else if (wavFile.numChannels() == 2 && wavFile.bitDepth() == 16) {
        format = AL_FORMAT_STEREO16;
    } else {
        PHENYL_LOGE(LOGGER, "Unsupported WAV file settings: channels={}, bit depth={}", wavFile.numChannels(), wavFile.bitDepth());
        return;
    }

    alBufferData(m_id, format, wavFile.data(), (int)wavFile.dataSize(), (int)wavFile.sampleRate());
    if ((err = alGetError())) {
        PHENYL_LOGE(LOGGER, "Failed to buffer data to OpenAL buffer: {}", ALStrError(err));
        return;
    }
    m_valid = true;
}

OpenALBuffer::OpenALBuffer (OpenALBuffer&& other) noexcept : m_id{other.m_id}, m_valid{other.m_valid} {
    other.m_id = 0;
    other.m_valid = false;
}

OpenALBuffer& OpenALBuffer::operator= (OpenALBuffer&& other) noexcept {
    if (m_valid) {
        alDeleteBuffers(1, &m_id);
    }

    m_id = other.m_id;
    m_valid = other.m_valid;

    other.m_id = 0;
    other.m_valid = false;
    return *this;
}

OpenALBuffer::~OpenALBuffer () {
    if (m_valid) {
        alDeleteBuffers(1, &m_id);
    }
}

float OpenALBuffer::duration () const {
    if (!m_valid) {
        PHENYL_LOGD(LOGGER, "Attempted to get duration for invalid buffer");
        return 0.0f;
    }
    ALint frequency, channels, bits, size;
    alGetBufferi(m_id, AL_FREQUENCY, &frequency);
    alGetBufferi(m_id, AL_CHANNELS, &channels);
    alGetBufferi(m_id, AL_BITS, &bits);
    alGetBufferi(m_id, AL_SIZE, &size);

    PHENYL_TRACE(LOGGER, "Buffer id={} freq={}, channels={}, bits={}, size={}", m_id, frequency, channels, bits, size);
    return ((float) size) / (float) (frequency * channels * bits / 8);
}
