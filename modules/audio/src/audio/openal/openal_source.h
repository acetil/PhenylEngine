#pragma once

#include "openal_headers.h"

namespace phenyl::audio {
class OpenALBuffer;

class OpenALSource {
public:
    OpenALSource ();

    OpenALSource (const OpenALSource&) = delete;
    OpenALSource (OpenALSource&& other) noexcept;

    OpenALSource& operator= (const OpenALSource&) = delete;
    OpenALSource& operator= (OpenALSource&& other) noexcept;

    ~OpenALSource ();

    explicit operator bool () const {
        return m_valid;
    }

    [[nodiscard]] ALuint id () const {
        return m_id;
    }

    void playBuffer (const OpenALBuffer& buffer);

    [[nodiscard]] float getGain () const;
    void setGain (float gain);

    void stop ();
    bool stopped () const;

private:
    ALuint m_id;
    bool m_valid;
};
} // namespace phenyl::audio
