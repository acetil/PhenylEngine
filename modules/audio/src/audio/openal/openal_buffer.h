#pragma once

#include "audio/filetypes/wav.h"
#include "openal_headers.h"

namespace phenyl::audio {
class OpenALBuffer {
public:
    OpenALBuffer () = default;
    explicit OpenALBuffer (const WAVFile& wavFile);

    OpenALBuffer (const OpenALBuffer&) = delete;
    OpenALBuffer (OpenALBuffer&& other) noexcept;

    OpenALBuffer& operator= (const OpenALBuffer&) = delete;
    OpenALBuffer& operator= (OpenALBuffer&& other) noexcept;

    explicit operator bool () const {
        return m_valid;
    }

    [[nodiscard]] ALuint id () const {
        return m_id;
    }

    [[nodiscard]] float duration () const;

    ~OpenALBuffer ();

private:
    ALuint m_id{0};
    bool m_valid{false};
};
} // namespace phenyl::audio
