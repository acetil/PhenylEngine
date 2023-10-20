#pragma once

#include "audio/filetypes/wav.h"

#include "openal_headers.h"

namespace phenyl::audio {
    class OpenALBuffer {
    private:
        ALuint bufferId{0};
        bool valid{false};
    public:
        OpenALBuffer () = default;
        explicit OpenALBuffer (const WAVFile& wavFile);

        OpenALBuffer (const OpenALBuffer&) = delete;
        OpenALBuffer (OpenALBuffer&& other) noexcept;

        OpenALBuffer& operator= (const OpenALBuffer&) = delete;
        OpenALBuffer& operator= (OpenALBuffer&& other) noexcept;

        explicit operator bool () const {
            return valid;
        }

        [[nodiscard]] ALuint id () const {
            return bufferId;
        }

        [[nodiscard]] float duration () const;

        ~OpenALBuffer ();
    };
}