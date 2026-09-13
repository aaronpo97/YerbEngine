#include <Configuration/ConfigReader.hpp>
#include <Configuration/ConfigStore.hpp>
#include <boost/test/unit_test.hpp>
#include <memory>

using namespace YerbEngine;

namespace {

/** Answers every key the way the JSON provider answers an unknown one. */
struct ZeroProvider final : IConfigProvider {
   ConfigValue getKey(std::string const&) const override { return 0; }
   bool hasKey(std::string const&) const override { return false; }
};

}  // namespace

struct ConfigReaderTestFixture {
   ConfigStore store{std::make_unique<ZeroProvider>()};
   ConfigReader reader{store};
};

BOOST_FIXTURE_TEST_SUITE(ConfigReaderTests, ConfigReaderTestFixture)

/*
 * A provider that reports a key as absent but still answers with 0 must not
 * drag the caller's fallback down to 0.
 */
BOOST_AUTO_TEST_CASE(absent_keys_return_the_fallback) {
   BOOST_CHECK_EQUAL(reader.intOr("fonts.size", 38), 38);
   BOOST_CHECK_EQUAL(reader.u64Or("spawn.interval", 500), 500u);
   BOOST_CHECK_CLOSE(reader.floatOr("ball.speed", 7.5f), 7.5f, 0.001f);
   BOOST_CHECK_EQUAL(reader.strOr("window.title", "YerbEngine"), "YerbEngine");
}

BOOST_AUTO_TEST_CASE(absent_colors_return_the_fallback) {
   constexpr SDL_Color fallback{.r = 1, .g = 2, .b = 3, .a = 4};

   SDL_Color const color = reader.colorOr("shape.color", fallback);

   BOOST_CHECK_EQUAL(color.r, fallback.r);
   BOOST_CHECK_EQUAL(color.g, fallback.g);
   BOOST_CHECK_EQUAL(color.b, fallback.b);
   BOOST_CHECK_EQUAL(color.a, fallback.a);
}

BOOST_AUTO_TEST_CASE(absent_vectors_return_the_fallback) {
   Vec2 const size = reader.vec2Or("window.size", Vec2{1600, 900});

   BOOST_CHECK_CLOSE(size.x(), 1600.0f, 0.001f);
   BOOST_CHECK_CLOSE(size.y(), 900.0f, 0.001f);
}

BOOST_AUTO_TEST_SUITE_END()
