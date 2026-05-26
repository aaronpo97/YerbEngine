cmake_minimum_required(VERSION 3.21...3.28)

set(DEMO_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
file(GLOB_RECURSE DEMO_SRC_FILES "${DEMO_DIR}/src/*.cpp")

add_library(demo_includes INTERFACE)
target_include_directories(demo_includes INTERFACE 
    "${CMAKE_SOURCE_DIR}/YerbEngine/includes"
    "${DEMO_DIR}/includes"
)

add_executable(shoot-demo ${DEMO_SRC_FILES})

target_include_directories(shoot-demo PRIVATE 
    "${CMAKE_SOURCE_DIR}/YerbEngine/includes"
    "${DEMO_DIR}/includes"
)

target_compile_definitions(shoot-demo PRIVATE DEMO_DIR="demo-1")

set_target_properties(shoot-demo PROPERTIES
        OUTPUT_NAME "demo-1"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

target_link_libraries(shoot-demo PRIVATE yerb_engine_core)

if(EMSCRIPTEN)
    set_target_properties(shoot-demo PROPERTIES
        OUTPUT_NAME "demo-1"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        CMAKE_EXECUTABLE_SUFFIX ".js"
    )

    target_link_options(shoot-demo PRIVATE
        -O3
        -fwasm-exceptions
        "-sUSE_SDL=2"
        "-sUSE_SDL_IMAGE=2" "-sUSE_LIBPNG=1" "-sUSE_LIBJPEG=1" "-sSDL2_IMAGE_FORMATS=['png','jpg']"
        "-sUSE_SDL_TTF=2" "-sUSE_FREETYPE=1"
        "-sUSE_SDL_MIXER=2"
        "-sALLOW_MEMORY_GROWTH=1"
        "--preload-file=${CMAKE_SOURCE_DIR}/assets@assets"
        "--preload-file=${CMAKE_SOURCE_DIR}/config@config"
    )
else()
    add_dependencies(shoot-demo copy_assets_runtime copy_config_runtime)

    add_custom_command(TARGET shoot-demo POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/assets" "${CMAKE_BINARY_DIR}/assets"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/config" "${CMAKE_BINARY_DIR}/config"
    )
endif()
