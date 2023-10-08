add_library(audio src/audio/audio_system.cpp
        src/audio/openal/openal_system.h
        src/audio/openal/openal_system.cpp
        src/audio/openal/openal_headers.h)

find_package(OpenAL REQUIRED)

target_include_directories(audio PUBLIC include)
target_include_directories(audio PRIVATE src)

target_link_libraries(audio PRIVATE OpenAL logger)