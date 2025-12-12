#include "Configuration/IConfigProvider.hpp"
#include "Timer.hpp"

#include <boost/test/unit_test.hpp>
#include <filesystem>
#include <memory>

using namespace YerbEngine;

BOOST_AUTO_TEST_SUITE(JsonConfigProviderTests)

BOOST_AUTO_TEST_CASE(test_load_valid_json_file) {
    Timer timer("Load valid JSON config file");

    std::filesystem::path configPath = "tests/data/test_config.json";
    BOOST_REQUIRE(std::filesystem::exists(configPath));

    BOOST_CHECK_NO_THROW(JsonConfigProvider provider(configPath));
}

BOOST_AUTO_TEST_CASE(test_load_nonexistent_file) {
    Timer timer("Load nonexistent JSON file throws exception");

    std::filesystem::path configPath = "nonexistent.json";

    BOOST_CHECK_THROW(JsonConfigProvider provider(configPath),
                      std::runtime_error);
}

BOOST_AUTO_TEST_CASE(test_get_simple_int_value) {
    Timer timer("Get simple int value");

    JsonConfigProvider provider("tests/data/test_config.json");
    ConfigValue        value = provider.getKey("testInt");

    BOOST_CHECK(std::holds_alternative<int>(value));
    BOOST_CHECK_EQUAL(std::get<int>(value), 42);
}

BOOST_AUTO_TEST_CASE(test_get_simple_float_value) {
    Timer timer("Get simple float value");

    JsonConfigProvider provider("tests/data/test_config.json");
    ConfigValue        value = provider.getKey("testFloat");

    BOOST_CHECK(std::holds_alternative<float>(value));
    BOOST_CHECK_CLOSE(std::get<float>(value), 3.14f, 0.001f);
}

BOOST_AUTO_TEST_CASE(test_get_simple_string_value) {
    Timer timer("Get simple string value");

    JsonConfigProvider provider("tests/data/test_config.json");
    ConfigValue        value = provider.getKey("testString");

    BOOST_CHECK(std::holds_alternative<std::string>(value));
    BOOST_CHECK_EQUAL(std::get<std::string>(value), "hello");
}

BOOST_AUTO_TEST_CASE(test_get_simple_bool_value) {
    Timer timer("Get simple bool value");

    JsonConfigProvider provider("tests/data/test_config.json");
    ConfigValue        value = provider.getKey("testBool");

    BOOST_CHECK(std::holds_alternative<bool>(value));
    BOOST_CHECK_EQUAL(std::get<bool>(value), true);
}

BOOST_AUTO_TEST_CASE(test_get_nested_int_value) {
    Timer timer("Get nested int value with dot notation");

    JsonConfigProvider provider("tests/data/test_config.json");
    ConfigValue        value = provider.getKey("window.width");

    BOOST_CHECK(std::holds_alternative<int>(value));
    BOOST_CHECK_EQUAL(std::get<int>(value), 1920);
}

BOOST_AUTO_TEST_CASE(test_get_nested_float_value) {
    Timer timer("Get nested float value with dot notation");

    JsonConfigProvider provider("tests/data/test_config.json");
    ConfigValue        value = provider.getKey("player.speed");

    BOOST_CHECK(std::holds_alternative<float>(value));
    BOOST_CHECK_CLOSE(std::get<float>(value), 5.5f, 0.001f);
}

BOOST_AUTO_TEST_CASE(test_get_nested_string_value) {
    Timer timer("Get nested string value with dot notation");

    JsonConfigProvider provider("tests/data/test_config.json");
    ConfigValue        value = provider.getKey("window.title");

    BOOST_CHECK(std::holds_alternative<std::string>(value));
    BOOST_CHECK_EQUAL(std::get<std::string>(value), "Test Game");
}

BOOST_AUTO_TEST_CASE(test_get_nested_bool_value) {
    Timer timer("Get nested bool value with dot notation");

    JsonConfigProvider provider("tests/data/test_config.json");
    ConfigValue        value = provider.getKey("window.fullscreen");

    BOOST_CHECK(std::holds_alternative<bool>(value));
    BOOST_CHECK_EQUAL(std::get<bool>(value), false);
}

BOOST_AUTO_TEST_CASE(test_get_sdl_color_value) {
    Timer timer("Get SDL_Color from JSON object");

    JsonConfigProvider provider("tests/data/test_config.json");
    ConfigValue        value = provider.getKey("player.color");

    BOOST_CHECK(std::holds_alternative<SDL_Color>(value));
    SDL_Color color = std::get<SDL_Color>(value);
    BOOST_CHECK_EQUAL(color.r, 255);
    BOOST_CHECK_EQUAL(color.g, 0);
    BOOST_CHECK_EQUAL(color.b, 0);
    BOOST_CHECK_EQUAL(color.a, 255);
}

BOOST_AUTO_TEST_CASE(test_get_nonexistent_key) {
    Timer timer("Get nonexistent key returns default");

    JsonConfigProvider provider("tests/data/test_config.json");
    ConfigValue        value = provider.getKey("nonexistent");

    // Should return default value (0)
    BOOST_CHECK(std::holds_alternative<int>(value));
    BOOST_CHECK_EQUAL(std::get<int>(value), 0);
}

BOOST_AUTO_TEST_CASE(test_get_nonexistent_nested_key) {
    Timer timer("Get nonexistent nested key returns default");

    JsonConfigProvider provider("tests/data/test_config.json");
    ConfigValue        value = provider.getKey("window.nonexistent");

    // Should return default value (0)
    BOOST_CHECK(std::holds_alternative<int>(value));
    BOOST_CHECK_EQUAL(std::get<int>(value), 0);
}

BOOST_AUTO_TEST_CASE(test_multiple_values) {
    Timer timer("Get multiple values from same provider");

    JsonConfigProvider provider("tests/data/test_config.json");

    ConfigValue width     = provider.getKey("window.width");
    ConfigValue height    = provider.getKey("window.height");
    ConfigValue speed     = provider.getKey("player.speed");
    ConfigValue volume    = provider.getKey("audio.volume");
    ConfigValue testInt   = provider.getKey("testInt");
    ConfigValue testFloat = provider.getKey("testFloat");

    BOOST_CHECK_EQUAL(std::get<int>(width), 1920);
    BOOST_CHECK_EQUAL(std::get<int>(height), 1080);
    BOOST_CHECK_CLOSE(std::get<float>(speed), 5.5f, 0.001f);
    BOOST_CHECK_CLOSE(std::get<float>(volume), 0.75f, 0.001f);
    BOOST_CHECK_EQUAL(std::get<int>(testInt), 42);
    BOOST_CHECK_CLOSE(std::get<float>(testFloat), 3.14f, 0.001f);
}

BOOST_AUTO_TEST_CASE(test_polymorphic_access) {
    Timer timer("Test polymorphic access through IConfigProvider interface");

    auto jsonProvider =
        std::make_unique<JsonConfigProvider>("tests/data/test_config.json");
    IConfigProvider *provider = jsonProvider.get();

    ConfigValue width  = provider->getKey("window.width");
    ConfigValue title  = provider->getKey("window.title");
    ConfigValue volume = provider->getKey("audio.volume");

    BOOST_CHECK_EQUAL(std::get<int>(width), 1920);
    BOOST_CHECK_EQUAL(std::get<std::string>(title), "Test Game");
    BOOST_CHECK_CLOSE(std::get<float>(volume), 0.75f, 0.001f);
}

BOOST_AUTO_TEST_SUITE_END()
