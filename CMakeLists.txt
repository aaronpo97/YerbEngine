cmake_minimum_required(VERSION 3.21...3.28)
project(YerbEngine VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
        json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
file(GLOB_RECURSE CORE_SRC_FILES 
    "${SRC_DIR}/*.cpp"
)

add_library(yerb_engine_core STATIC ${CORE_SRC_FILES})

target_include_directories(yerb_engine_core PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
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

    target_compile_options(yerb_engine_core PUBLIC
        "-sUSE_SDL=2"
        "-sUSE_SDL_IMAGE=2"
        "-sUSE_SDL_TTF=2"
        "-sUSE_SDL_MIXER=2"
    )

    # Add SDL include dirs from Emscripten
    execute_process(
        COMMAND ${CMAKE_C_COMPILER} -print-file-name=include
        OUTPUT_VARIABLE EMSCRIPTEN_INCLUDE_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    target_include_directories(yerb_engine_core PUBLIC ${EMSCRIPTEN_INCLUDE_DIR})

    if(NOT TARGET copy_templates)
        add_custom_target(copy_templates ALL
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/template" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
        )
    endif()

else()
    message(STATUS "Configuring for native")

    find_package(SDL2       REQUIRED)
    find_package(SDL2_image REQUIRED)
    find_package(SDL2_ttf   REQUIRED)
    find_package(SDL2_mixer REQUIRED)

    target_link_libraries(yerb_engine_core PUBLIC
        SDL2::SDL2
        SDL2_image::SDL2_image
        SDL2_ttf::SDL2_ttf
        SDL2_mixer::SDL2_mixer
    )

    set(OUT_DIR "${CMAKE_BINARY_DIR}")

    if(NOT TARGET copy_assets_runtime)
        add_custom_target(copy_assets_runtime ALL
            COMMAND ${CMAKE_COMMAND} -E make_directory "${OUT_DIR}/assets"
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/assets" "${OUT_DIR}/assets"
        )
    endif()

    if(NOT TARGET copy_config_runtime)
        add_custom_target(copy_config_runtime ALL
            COMMAND ${CMAKE_COMMAND} -E make_directory "${OUT_DIR}/config"
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/config" "${OUT_DIR}/config"
        )
    endif()
endif()

add_subdirectory(shoot-demo)
