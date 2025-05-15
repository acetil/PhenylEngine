#pragma once

#include "AL/al.h"
#include "AL/alc.h"

namespace phenyl::audio {
static inline const char* ALStrError (ALenum err) {
    switch (err) {
    case AL_NO_ERROR:
        return "No Error";
    case AL_INVALID_NAME:
        return "Invalid Name";
    case AL_INVALID_ENUM:
        return "Invalid Enum";
    case AL_INVALID_VALUE:
        return "Invalid Value";
    case AL_INVALID_OPERATION:
        return "Invalid Operation";
    case AL_OUT_OF_MEMORY:
        return "Out Of Memory";
    default:
        return "Unknown Error";
    }
}

static inline const char* ALcStrError (ALCenum err) {
    switch (err) {
    case ALC_NO_ERROR:
        return "No Error";
    case ALC_INVALID_DEVICE:
        return "Invalid Device";
    case ALC_INVALID_CONTEXT:
        return "Invalid Context";
    case ALC_INVALID_ENUM:
        return "Invalid Enum";
    case ALC_INVALID_VALUE:
        return "Invalid Value";
    case AL_OUT_OF_MEMORY:
        return "Out Of Memory";
    default:
        return "Unknown Error";
    }
}
} // namespace phenyl::audio
