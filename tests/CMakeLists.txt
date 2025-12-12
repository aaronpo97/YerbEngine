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

# Copy assets and config for tests
add_custom_command(TARGET yerb_engine_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets" "${CMAKE_CURRENT_BINARY_DIR}/assets"
    
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/config" "${CMAKE_CURRENT_BINARY_DIR}/config"
    
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/data" "${CMAKE_CURRENT_BINARY_DIR}/tests/data"
)

# Register with CTest
add_test(NAME yerb_engine_tests COMMAND yerb_engine_tests)
