#include "Timer.hpp"
#include "Configuration/ConfigDictionary.hpp"

#include <boost/test/unit_test.hpp>

using namespace YerbEngine;

BOOST_AUTO_TEST_SUITE(ConfigDictionaryTests)

    // Test setting and getting various types
    BOOST_AUTO_TEST_CASE(test_set_get_various_types) {

        ConfigDictionary config;

        {
            Timer timer("Set int, float, string, bool, SDL_Color");
            config.set<int>("screen_width", 1920);
            config.set<float>("volume", 0.75f);
            config.set<std::string>("player_name", "Hero");
            config.set<bool>("fullscreen", true);
            SDL_Color color = {255, 0, 0, 255};
            config.set<SDL_Color>("background_color", color);
        }

        {
            Timer timer("Verify values");
            auto  screenWidth = config.get<int>("screen_width");
            auto  volume      = config.get<float>("volume");
            auto  playerName  = config.get<std::string>("player_name");
            auto  fullscreen  = config.get<bool>("fullscreen");
            auto  bgColor     = config.get<SDL_Color>("background_color");

            BOOST_CHECK(screenWidth.has_value());
            BOOST_CHECK(volume.has_value());
            BOOST_CHECK(playerName.has_value());
            BOOST_CHECK(fullscreen.has_value());
            BOOST_CHECK(bgColor.has_value());

            BOOST_CHECK_CLOSE(volume.value(), 0.75f, 0.001f);
            BOOST_CHECK_EQUAL(playerName.value(), "Hero");
            BOOST_CHECK_EQUAL(fullscreen.value(), true);
            BOOST_CHECK_EQUAL(screenWidth.value(), 1920);
            BOOST_CHECK_EQUAL(bgColor.value().r, 255);
            BOOST_CHECK_EQUAL(bgColor.value().g, 0);
            BOOST_CHECK_EQUAL(bgColor.value().b, 0);
            BOOST_CHECK_EQUAL(bgColor.value().a, 255);
        }

    }
}