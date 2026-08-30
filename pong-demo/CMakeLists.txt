cmake_minimum_required(VERSION 3.21...3.28)

set(PONG_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
file(GLOB_RECURSE PONG_SRC_FILES "${PONG_DIR}/src/*.cpp")

add_executable(pong-demo ${PONG_SRC_FILES})

target_include_directories(pong-demo PRIVATE
    "${ENGINE_INCLUDE_DIR}"
    "${PONG_DIR}/includes"
)

target_compile_definitions(pong-demo PRIVATE DEMO_DIR="pong")

set_target_properties(pong-demo PROPERTIES
        OUTPUT_NAME "pong"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

target_link_libraries(pong-demo PRIVATE yerb_engine_core)

if(EMSCRIPTEN)
    set_target_properties(pong-demo PROPERTIES
        OUTPUT_NAME "pong"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        CMAKE_EXECUTABLE_SUFFIX ".js"
    )

    target_link_options(pong-demo PRIVATE
        -O3
        -fwasm-exceptions
        "-sALLOW_MEMORY_GROWTH=1"
        "--preload-file=${PONG_DIR}/assets@pong-assets"
        "--preload-file=${PONG_DIR}/config@pong-config"
    )
else()
    add_custom_command(TARGET pong-demo POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${PONG_DIR}/assets" "${CMAKE_BINARY_DIR}/pong-assets"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${PONG_DIR}/config" "${CMAKE_BINARY_DIR}/pong-config"
    )
endif()
