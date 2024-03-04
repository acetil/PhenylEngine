add_library(audio src/audio/audio_system.cpp
        src/audio/openal/openal_system.h
        src/audio/openal/openal_system.cpp
        src/audio/openal/openal_headers.h
        src/audio/filetypes/wav.h
        src/audio/filetypes/wav.cpp
        include/audio/audio_source.h
        src/audio/audio_source.cpp
        src/audio/openal/openal_source.h
        src/audio/openal/openal_source.cpp
        include/audio/audio_sample.h
        src/audio/audio_sample.cpp
        src/audio/openal/openal_buffer.h
        src/audio/openal/openal_buffer.cpp
        include/audio/components/audio_player.h
        src/audio/components/audio_player.cpp
        include/audio/audio_backend.h
        include/audio/detail/loggers.h
        src/audio/detail/loggers.cpp)

find_package(OpenAL REQUIRED)
set_property(TARGET audio PROPERTY CXX_STANDARD 20)

target_include_directories(audio PUBLIC include)
target_include_directories(audio PRIVATE src)

target_link_libraries(audio PRIVATE OpenAL::OpenAL logger util common component)