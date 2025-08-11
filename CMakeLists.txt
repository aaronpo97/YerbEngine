cmake_minimum_required(VERSION 3.21...3.28)
project(SDL_GAME VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT EMSCRIPTEN)
        find_package(SDL2 REQUIRED)
        find_package(SDL2_image REQUIRED)
        find_package(SDL2_ttf REQUIRED)
        find_package(SDL2_mixer REQUIRED)
endif()

include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
file(GLOB_RECURSE SRC_FILES "${SRC_DIR}/*.cpp")

add_executable(sdl_game_app ${SRC_FILES})

set_target_properties(sdl_game_app PROPERTIES
        OUTPUT_NAME "SDL_GAME"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

if(APPLE)
        target_compile_options(sdl_game_app PRIVATE -Wno-deprecated -Wno-deprecated-declarations)
endif()

if(EMSCRIPTEN)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s DISABLE_EXCEPTION_CATCHING=0 -O3")
        set_target_properties(sdl_game_app PROPERTIES CMAKE_EXECUTABLE_SUFFIX ".js")

        target_compile_options(sdl_game_app PRIVATE
                "-sUSE_SDL=2" "-sUSE_SDL_IMAGE=2" "-sUSE_SDL_TTF=2" "-sUSE_SDL_MIXER=2"
        )
        target_link_options(sdl_game_app PRIVATE
                "-sUSE_SDL=2"
                "-sUSE_SDL_IMAGE=2" "-sUSE_LIBPNG=1" "-sSDL2_IMAGE_FORMATS=['png']"
                "-sUSE_SDL_TTF=2"
                "-sUSE_SDL_MIXER=2"
                "-sALLOW_MEMORY_GROWTH=1"
                "--preload-file=${CMAKE_SOURCE_DIR}/assets@assets"
                "--preload-file=${CMAKE_SOURCE_DIR}/config@config"
        )

        if(NOT TARGET copy_templates)
                add_custom_target(copy_templates ALL
                        COMMAND ${CMAKE_COMMAND} -E copy_directory
                        "${CMAKE_SOURCE_DIR}/template" "${CMAKE_BINARY_DIR}"
                        COMMENT "Copying template directory to ${CMAKE_BINARY_DIR}"
                )
        endif()
else()
        target_link_libraries(sdl_game_app PRIVATE
                SDL2::SDL2
                SDL2_image::SDL2_image
                SDL2_ttf::SDL2_ttf
                SDL2_mixer::SDL2_mixer
                nlohmann_json::nlohmann_json
        )

        set(OUT_DIR "${CMAKE_BINARY_DIR}")

        if(NOT TARGET copy_assets_runtime)
                add_custom_target(copy_assets_runtime ALL
                        COMMAND ${CMAKE_COMMAND} -E make_directory "${OUT_DIR}/assets"
                        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/assets" "${OUT_DIR}/assets"
                        COMMENT "Copying assets -> ${OUT_DIR}/assets"
                )
        endif()

        if(NOT TARGET copy_config_runtime)
                add_custom_target(copy_config_runtime ALL
                        COMMAND ${CMAKE_COMMAND} -E make_directory "${OUT_DIR}/config"
                        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/config" "${OUT_DIR}/config"
                        COMMENT "Copying config -> ${OUT_DIR}/config"
                )
        endif()

        add_dependencies(sdl_game_app copy_assets_runtime copy_config_runtime)

        add_custom_command(TARGET sdl_game_app POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/assets" "${OUT_DIR}/assets"
                COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/config" "${OUT_DIR}/config"
                COMMENT "Syncing runtime assets to ${OUT_DIR}"
        )
endif()
