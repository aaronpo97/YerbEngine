#include <AssetManagement/TextureManager.hpp>
#include <boost/test/unit_test.hpp>
#include <filesystem>
#include <memory>

using namespace YerbEngine;

namespace {
    std::filesystem::path getDemoAssetsRoot() {
#ifdef PROJECT_ASSETS_DIR
        return std::filesystem::path(PROJECT_ASSETS_DIR);
#else
        return {};
#endif
    }
} // namespace

struct TextureManagerTestFixture {
    SDL_Surface                    *surface{nullptr};
    SDL_Renderer                   *renderer{nullptr};
    std::unique_ptr<TextureManager> manager;

    TextureManagerTestFixture() {
        SDL_SetHint(SDL_HINT_VIDEODRIVER, "dummy");
        SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");
        BOOST_REQUIRE_EQUAL(SDL_Init(SDL_INIT_VIDEO), 0);
        surface = SDL_CreateRGBSurfaceWithFormat(0, 32, 32, 32,
                                                 SDL_PIXELFORMAT_RGBA32);
        BOOST_REQUIRE(surface != nullptr);

        renderer = SDL_CreateSoftwareRenderer(surface);
        BOOST_REQUIRE(renderer != nullptr);

        manager = std::make_unique<TextureManager>(renderer);
    }

    ~TextureManagerTestFixture() {
        manager.reset();

        if (renderer != nullptr) {
            SDL_DestroyRenderer(renderer);
        }

        if (surface != nullptr) {
            SDL_FreeSurface(surface);
        }

        SDL_Quit();
    }
};

BOOST_AUTO_TEST_SUITE(TextureManagerTests)

BOOST_FIXTURE_TEST_CASE(registers_and_retrieves_texture,
                        TextureManagerTestFixture) {
    auto assetRoot = getDemoAssetsRoot();
    BOOST_REQUIRE(!assetRoot.empty());
    std::filesystem::path playerTexture = assetRoot / "images" / "player.png";
    BOOST_REQUIRE(std::filesystem::exists(playerTexture));

    manager->registerTexture("player", playerTexture);

    BOOST_CHECK(manager->hasTexture("player"));
    SDL_Texture *texture = manager->getTexture("player");
    BOOST_REQUIRE(texture != nullptr);
}

BOOST_FIXTURE_TEST_CASE(duplicate_registration_is_noop,
                        TextureManagerTestFixture) {
    auto assetRoot = getDemoAssetsRoot();
    BOOST_REQUIRE(!assetRoot.empty());
    std::filesystem::path playerTexture = assetRoot / "images" / "player.png";
    BOOST_REQUIRE(std::filesystem::exists(playerTexture));

    manager->registerTexture("player", playerTexture);
    SDL_Texture *first = manager->getTexture("player");
    BOOST_REQUIRE(first != nullptr);

    manager->registerTexture("player", playerTexture);

    SDL_Texture *second = manager->getTexture("player");
    BOOST_CHECK_EQUAL(first, second);
}

BOOST_FIXTURE_TEST_CASE(missing_texture_returns_null,
                        TextureManagerTestFixture) {
    BOOST_CHECK(!manager->hasTexture("ghost"));
    BOOST_CHECK(manager->getTexture("ghost") == nullptr);

    manager->registerTexture("ghost", "path/to/missing.png");

    BOOST_CHECK(!manager->hasTexture("ghost"));
    BOOST_CHECK(manager->getTexture("ghost") == nullptr);
}

BOOST_AUTO_TEST_SUITE_END()
