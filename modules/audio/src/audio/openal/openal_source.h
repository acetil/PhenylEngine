#pragma once

#include "openal_headers.h"

namespace phenyl::audio {
    class OpenALBuffer;
    class OpenALSource {
    private:
        ALuint sourceId;
        bool valid;
    public:
        OpenALSource ();

        explicit operator bool () const {
            return valid;
        }

        OpenALSource (const OpenALSource&) = delete;
        OpenALSource (OpenALSource&& other) noexcept;

        OpenALSource& operator= (const OpenALSource&) = delete;
        OpenALSource& operator= (OpenALSource&& other) noexcept;

        [[nodiscard]] ALuint id () const {
            return sourceId;
        }

        void playBuffer (const OpenALBuffer& buffer);

        ~OpenALSource();
    };
}