cmake_minimum_required(VERSION 3.21...3.28)
project(YerbEngine VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(YERB_BUILD_DEMOS "Build the games that ship with the engine" ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(YerbAddGame)

# Enable testing
enable_testing()

# Keep runtime outputs in the top-level build directory for single-config generators.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}")

include(FetchContent)
FetchContent_Declare(
        json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)

set(ENGINE_SRC_DIR "${CMAKE_SOURCE_DIR}/lib/src")
set(ENGINE_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/lib/includes")

file(GLOB_RECURSE CORE_SRC_FILES 
    "${ENGINE_SRC_DIR}/*.cpp"
)

add_library(yerb_engine_core STATIC ${CORE_SRC_FILES})

target_include_directories(yerb_engine_core PUBLIC 
    $<BUILD_INTERFACE:${ENGINE_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(yerb_engine_core PUBLIC nlohmann_json::nlohmann_json)

if(APPLE)
    target_compile_options(yerb_engine_core PRIVATE -Wno-deprecated -Wno-deprecated-declarations)
endif()

if(EMSCRIPTEN)
    message(STATUS "Configuring for Emscripten")

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

    # Emscripten carries ports for SDL3 and SDL3_ttf. SDL3_image and SDL3_mixer
    # have no port, so they are built from source against the SDL3 port's
    # headers: both skip their own find_package(SDL3) when the SDL3::SDL3 and
    # SDL3::Headers targets already exist, which sdl3_port provides below.
    add_library(sdl3_port INTERFACE)
    target_compile_options(sdl3_port INTERFACE "--use-port=sdl3")
    target_link_options(sdl3_port INTERFACE "--use-port=sdl3")
    add_library(SDL3::SDL3 ALIAS sdl3_port)
    add_library(SDL3::Headers ALIAS sdl3_port)

    # PNG and JPEG both go through SDL3_image's stb backend, and Ogg Vorbis
    # through SDL3_mixer's stb_vorbis, so neither pulls in an external codec.
    set(SDLIMAGE_VENDORED     OFF CACHE BOOL "" FORCE)
    set(SDLIMAGE_SAMPLES      OFF CACHE BOOL "" FORCE)
    set(SDLIMAGE_TESTS        OFF CACHE BOOL "" FORCE)
    set(SDLIMAGE_BACKEND_STB  ON  CACHE BOOL "" FORCE)
    set(SDLIMAGE_PNG_LIBPNG   OFF CACHE BOOL "" FORCE)
    foreach(fmt ANI AVIF BMP GIF JXL LBM PCX PNM QOI SVG TGA TIF WEBP XCF XPM XV)
        set(SDLIMAGE_${fmt} OFF CACHE BOOL "" FORCE)
    endforeach()
    set(SDLIMAGE_PNG ON CACHE BOOL "" FORCE)
    set(SDLIMAGE_JPG ON CACHE BOOL "" FORCE)

    set(SDLMIXER_VENDORED            OFF CACHE BOOL "" FORCE)
    set(SDLMIXER_TESTS               OFF CACHE BOOL "" FORCE)
    set(SDLMIXER_EXAMPLES            OFF CACHE BOOL "" FORCE)
    foreach(fmt AIFF VOC AU FLAC GME MOD MP3 MIDI OPUS WAVPACK VORBIS_VORBISFILE)
        set(SDLMIXER_${fmt} OFF CACHE BOOL "" FORCE)
    endforeach()
    set(SDLMIXER_WAVE       ON CACHE BOOL "" FORCE)
    set(SDLMIXER_VORBIS_STB ON CACHE BOOL "" FORCE)

    FetchContent_Declare(sdl3_image
        URL https://github.com/libsdl-org/SDL_image/releases/download/release-3.4.8/SDL3_image-3.4.8.tar.gz
    )
    FetchContent_Declare(sdl3_mixer
        URL https://github.com/libsdl-org/SDL_mixer/releases/download/release-3.2.4/SDL3_mixer-3.2.4.tar.gz
    )
    FetchContent_MakeAvailable(sdl3_image sdl3_mixer)

    target_compile_options(yerb_engine_core PUBLIC
        "--use-port=sdl3"
        "--use-port=sdl3_ttf"
    )
    target_link_libraries(yerb_engine_core PUBLIC
        SDL3_image::SDL3_image
        SDL3_mixer::SDL3_mixer
    )

    if(NOT TARGET copy_templates)
        add_custom_target(copy_templates ALL
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/template" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
        )
    endif()

else()
    message(STATUS "Configuring for native")

    find_package(SDL3       REQUIRED CONFIG)
    find_package(SDL3_image REQUIRED CONFIG)
    find_package(SDL3_ttf   REQUIRED CONFIG)
    find_package(SDL3_mixer REQUIRED CONFIG)

    target_link_libraries(yerb_engine_core PUBLIC
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_ttf::SDL3_ttf
        SDL3_mixer::SDL3_mixer
    )

endif()

# Boost.Test setup (only for native builds)
if(NOT EMSCRIPTEN)
    find_package(Boost COMPONENTS unit_test_framework)
    
    if(Boost_FOUND)
        message(STATUS "Boost.Test found - tests will be built")
        add_subdirectory(tests)
    else()
        message(WARNING "Boost.Test not found - tests will be skipped")
        message(WARNING "Install with: brew install boost (macOS) or apt-get install libboost-test-dev (Linux)")
    endif()
endif()

if(YERB_BUILD_DEMOS)
    add_subdirectory(shoot-demo)
    add_subdirectory(pong-demo)
endif()
