add_library(audio src/audio/audio_system.cpp
        src/audio/openal/openal_system.h
        src/audio/openal/openal_system.cpp
        src/audio/openal/openal_headers.h
        src/audio/filetypes/wav.h
        src/audio/filetypes/wav.cpp)

find_package(OpenAL REQUIRED)
set_property(TARGET audio PROPERTY CXX_STANDARD 20)

target_include_directories(audio PUBLIC include)
target_include_directories(audio PRIVATE src)

target_link_libraries(audio PRIVATE OpenAL logger util)