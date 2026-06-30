# Test executable configuration
file(GLOB_RECURSE TEST_SOURCES "*.cpp")

add_executable(yerb_engine_tests ${TEST_SOURCES})

target_link_libraries(yerb_engine_tests PRIVATE
    yerb_engine_core
    Boost::unit_test_framework
)

target_include_directories(yerb_engine_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/lib/includes
)

target_compile_definitions(yerb_engine_tests PRIVATE
    PROJECT_ASSETS_DIR="${CMAKE_SOURCE_DIR}/shoot-demo/assets"
)

set(TEST_ASSETS_SRC "${CMAKE_SOURCE_DIR}/shoot-demo/assets")
set(TEST_CONFIG_SRC "${CMAKE_SOURCE_DIR}/config")
set(TEST_DATA_SRC   "${CMAKE_CURRENT_SOURCE_DIR}/data")

# Copy assets if available for tests
if(EXISTS "${TEST_ASSETS_SRC}")
    add_custom_command(TARGET yerb_engine_tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${TEST_ASSETS_SRC}" "${CMAKE_CURRENT_BINARY_DIR}/assets"
    )
else()
    message(WARNING "Test assets directory not found at ${TEST_ASSETS_SRC} - skipping copy.")
endif()

add_custom_command(TARGET yerb_engine_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${TEST_CONFIG_SRC}" "${CMAKE_CURRENT_BINARY_DIR}/config"
)

if(EXISTS "${TEST_DATA_SRC}")
    add_custom_command(TARGET yerb_engine_tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${TEST_DATA_SRC}" "${CMAKE_CURRENT_BINARY_DIR}/tests/data"
    )
endif()

# Register with CTest
add_test(NAME yerb_engine_tests COMMAND yerb_engine_tests)
