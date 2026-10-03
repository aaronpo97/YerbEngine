include_guard(GLOBAL)

# Declares a game executable built on yerb_engine_core.
#
# yerb_add_game(
#     NAME        <target>        # CMake target name
#     OUTPUT_NAME <binary>        # name of the produced executable
#     ASSETS      <dir>           # asset directory to ship with the game
#     ASSETS_AT   <name>          # name the game reads assets under at runtime
#     CONFIG      <dir>           # config directory to ship with the game
#     CONFIG_AT   <name>          # name the game reads config under at runtime
#     [SOURCE_DIR <dir>]          # defaults to the calling directory
# )
#
# Sources are globbed from <SOURCE_DIR>/src, headers come from
# <SOURCE_DIR>/includes. Native builds copy the asset and config directories
# next to the executable after linking; Emscripten builds preload them into
# the virtual filesystem under the same names.
function(yerb_add_game)
    set(oneValueArgs NAME OUTPUT_NAME SOURCE_DIR ASSETS ASSETS_AT CONFIG CONFIG_AT)
    cmake_parse_arguments(GAME "" "${oneValueArgs}" "" ${ARGN})

    foreach(required NAME OUTPUT_NAME ASSETS ASSETS_AT CONFIG CONFIG_AT)
        if(NOT GAME_${required})
            message(FATAL_ERROR "yerb_add_game: ${required} is required")
        endif()
    endforeach()

    if(NOT GAME_SOURCE_DIR)
        set(GAME_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    endif()

    file(GLOB_RECURSE game_sources "${GAME_SOURCE_DIR}/src/*.cpp")
    if(NOT game_sources)
        message(FATAL_ERROR "yerb_add_game: no sources under ${GAME_SOURCE_DIR}/src")
    endif()

    add_executable(${GAME_NAME} ${game_sources})

    # yerb_engine_core exports its own include directory.
    target_include_directories(${GAME_NAME} PRIVATE "${GAME_SOURCE_DIR}/includes")
    target_link_libraries(${GAME_NAME} PRIVATE yerb_engine_core)

    set_target_properties(${GAME_NAME} PROPERTIES
        OUTPUT_NAME "${GAME_OUTPUT_NAME}"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    )

    if(EMSCRIPTEN)
        set_target_properties(${GAME_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
            SUFFIX ".js"
        )

        target_link_options(${GAME_NAME} PRIVATE
            -O3
            -fwasm-exceptions
            "--use-port=sdl3"
            "--use-port=sdl3_ttf"
            "-sALLOW_MEMORY_GROWTH=1"
            "--preload-file=${GAME_ASSETS}@${GAME_ASSETS_AT}"
            "--preload-file=${GAME_CONFIG}@${GAME_CONFIG_AT}"
        )
    else()
        add_custom_command(TARGET ${GAME_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${GAME_ASSETS}" "${CMAKE_BINARY_DIR}/${GAME_ASSETS_AT}"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${GAME_CONFIG}" "${CMAKE_BINARY_DIR}/${GAME_CONFIG_AT}"
        )
    endif()
endfunction()
