#include "Configuration/ConfigStore.hpp"
#include "Configuration/IConfigProvider.hpp"
#include "Timer.hpp"

#include <boost/test/unit_test.hpp>
#include <memory>

using namespace YerbEngine;

BOOST_AUTO_TEST_SUITE(ConfigStoreTests)

BOOST_AUTO_TEST_CASE(test_construct_with_valid_provider) {
    Timer timer("Construct ConfigStore with valid provider");

    auto provider =
        std::make_unique<JsonConfigProvider>("tests/data/test_config.json");
    BOOST_CHECK_NO_THROW(ConfigStore store(std::move(provider)));
}

BOOST_AUTO_TEST_CASE(test_construct_with_null_provider) {
    Timer timer("Construct ConfigStore with null provider throws");

    std::unique_ptr<IConfigProvider> nullProvider;
    BOOST_CHECK_THROW(ConfigStore store(std::move(nullProvider)),
                      std::runtime_error);
}

BOOST_AUTO_TEST_CASE(test_get_simple_value) {
    Timer timer("Get simple value from ConfigStore");

    auto provider =
        std::make_unique<JsonConfigProvider>("tests/data/test_config.json");
    ConfigStore store(std::move(provider));

    ConfigValue value = store.get("testInt");

    BOOST_CHECK(std::holds_alternative<int>(value));
    BOOST_CHECK_EQUAL(std::get<int>(value), 42);
}

BOOST_AUTO_TEST_CASE(test_get_nested_value) {
    Timer timer("Get nested value from ConfigStore");

    auto provider =
        std::make_unique<JsonConfigProvider>("tests/data/test_config.json");
    ConfigStore store(std::move(provider));

    ConfigValue width = store.get("window.width");
    ConfigValue title = store.get("window.title");

    BOOST_CHECK_EQUAL(std::get<int>(width), 1920);
    BOOST_CHECK_EQUAL(std::get<std::string>(title), "Test Game");
}

BOOST_AUTO_TEST_CASE(test_get_multiple_values) {
    Timer timer("Get multiple values from ConfigStore");

    auto provider =
        std::make_unique<JsonConfigProvider>("tests/data/test_config.json");
    ConfigStore store(std::move(provider));

    ConfigValue width       = store.get("window.width");
    ConfigValue height      = store.get("window.height");
    ConfigValue speed       = store.get("player.speed");
    ConfigValue volume      = store.get("audio.volume");
    ConfigValue playerColor = store.get("player.color");

    BOOST_CHECK_EQUAL(std::get<int>(width), 1920);
    BOOST_CHECK_EQUAL(std::get<int>(height), 1080);
    BOOST_CHECK_CLOSE(std::get<float>(speed), 5.5f, 0.001f);
    BOOST_CHECK_CLOSE(std::get<float>(volume), 0.75f, 0.001f);

    SDL_Color color = std::get<SDL_Color>(playerColor);
    BOOST_CHECK_EQUAL(color.r, 255);
    BOOST_CHECK_EQUAL(color.g, 0);
    BOOST_CHECK_EQUAL(color.b, 0);
    BOOST_CHECK_EQUAL(color.a, 255);
}

BOOST_AUTO_TEST_CASE(test_get_nonexistent_value) {
    Timer timer("Get nonexistent value returns default");

    ConfigStore store(std::move(
        std::make_unique<JsonConfigProvider>("tests/data/test_config.json")));

    ConfigValue value = store.get("nonexistent.key");

    BOOST_CHECK(std::holds_alternative<int>(value));
    BOOST_CHECK_EQUAL(std::get<int>(value), 0);
}

BOOST_AUTO_TEST_CASE(test_reload) {
    Timer timer("Reload ConfigStore");

    ConfigStore store(std::move(
        std::make_unique<JsonConfigProvider>("tests/data/test_config.json")));

    BOOST_CHECK_NO_THROW(store.reload());

    // Should still be able to get values after reload
    ConfigValue value = store.get("testInt");
    BOOST_CHECK_EQUAL(std::get<int>(value), 42);
}

BOOST_AUTO_TEST_SUITE_END()
