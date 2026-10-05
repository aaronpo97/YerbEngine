#include <SDL3/SDL.h>

#include <AssetManagement/AudioSampleBuffer.hpp>
#include <AssetManagement/FontManager.hpp>
#include <AssetManagement/TextureManager.hpp>
#include <Configuration/ConfigAdapter.hpp>
#include <Configuration/ConfigStore.hpp>
#include <GameScenes/Scene.hpp>
#include <GameScenes/SceneServices.hpp>
#include <SystemManagement/AudioManager.hpp>
#include <SystemManagement/VideoManager.hpp>
#include <boost/test/unit_test.hpp>
#include <memory>
#include <string>
#include <vector>

using namespace YerbEngine;

namespace {

/** Records transitions instead of performing them. */
struct FakeRouter : ISceneRouter {
   std::vector<std::string> loaded;
   bool quitCalled = false;

   void LoadScene(std::string const& sceneName,
                  std::shared_ptr<Scene> const& scene) override {
      BOOST_REQUIRE(scene != nullptr);
      loaded.push_back(sceneName);
   }

   void quit() override { quitCalled = true; }
};

struct FakeClock : IClock {
   Uint64 now = 0;
   Uint64 nowMs() const override { return now; }
};

/** A scene that does nothing but route, so the base class is what is tested. */
struct RoutingScene final : Scene {
   using Scene::Scene;

   void update() override {}
   void onEnd() override {}
   void sRender() override {}
};

}  // namespace

/*
 * Video and fonts are still concrete SDL-backed types, so the fixture stands
 * up real ones against SDL's dummy drivers. The router and the clock are
 * fakes, which is what lets a scene exist here without a GameEngine at all.
 */
struct SceneTestFixture {
   std::unique_ptr<ConfigStore> store;
   std::unique_ptr<ConfigAdapter> config;
   std::unique_ptr<VideoManager> video;
   std::unique_ptr<TextureManager> textures;
   std::unique_ptr<AudioManager> mixer;
   std::unique_ptr<AudioSampleBuffer> audio;
   std::unique_ptr<FontManager> fonts;
   FakeClock clock;
   FakeRouter router;
   std::unique_ptr<SceneServices> services;

   SceneTestFixture() {
      SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "dummy");
      SDL_SetHint(SDL_HINT_AUDIO_DRIVER, "dummy");

      store = std::make_unique<ConfigStore>(
          std::make_unique<EmptyConfigProvider>());
      config = std::make_unique<ConfigAdapter>(*store);

      video = std::make_unique<VideoManager>(*config);
      textures = std::make_unique<TextureManager>(
          RendererHandle{video->getRenderer()});
      mixer = std::make_unique<AudioManager>();
      audio = std::make_unique<AudioSampleBuffer>(*mixer, clock);

      auto const gameCfg = config->getGameConfig();
      fonts = std::make_unique<FontManager>(
          gameCfg.fontPath, FontSizes{.sm = gameCfg.fontSizeSm,
                                      .md = gameCfg.fontSizeMd,
                                      .lg = gameCfg.fontSizeLg});

      services =
          std::make_unique<SceneServices>(SceneServices{.video = *video,
                                                        .fonts = *fonts,
                                                        .textures = *textures,
                                                        .audio = *audio,
                                                        .mixer = *mixer,
                                                        .clock = clock,
                                                        .router = router});
   }

   ~SceneTestFixture() {
      services.reset();
      fonts.reset();
      audio.reset();
      mixer.reset();
      textures.reset();
      video.reset();
      SDL_Quit();
   }

   /** Delivers one key event to the scene, as the engine's input loop would. */
   static void press(Scene& scene, std::string const& actionName) {
      Action action(actionName, ActionState::START, std::nullopt);
      scene.sDoAction(action);
   }

   static void release(Scene& scene, std::string const& actionName) {
      Action action(actionName, ActionState::END, std::nullopt);
      scene.sDoAction(action);
   }
};

BOOST_FIXTURE_TEST_SUITE(SceneDispatchTests, SceneTestFixture)

BOOST_AUTO_TEST_CASE(a_scene_needs_no_game_engine) {
   RoutingScene scene{*services};

   BOOST_CHECK(router.loaded.empty());
   BOOST_CHECK(!router.quitCalled);
}

BOOST_AUTO_TEST_CASE(a_bound_action_routes_to_the_next_scene) {
   RoutingScene scene{*services};
   scene.bindPress("SELECT", [this, &scene](Action&) {
      services->router.LoadScene("Match",
                                 std::make_shared<RoutingScene>(*services));
   });

   press(scene, "SELECT");

   std::vector<std::string> const expected{"Match"};
   BOOST_TEST(router.loaded == expected, boost::test_tools::per_element());
}

BOOST_AUTO_TEST_CASE(a_bound_action_can_quit) {
   RoutingScene scene{*services};
   scene.bindPress("EXIT", [this](Action&) { services->router.quit(); });

   press(scene, "EXIT");

   BOOST_CHECK(router.quitCalled);
}

BOOST_AUTO_TEST_CASE(press_handlers_ignore_the_release) {
   RoutingScene scene{*services};
   int calls = 0;
   scene.bindPress("SELECT", [&calls](Action&) { ++calls; });

   press(scene, "SELECT");
   release(scene, "SELECT");

   BOOST_CHECK_EQUAL(calls, 1);
}

BOOST_AUTO_TEST_CASE(hold_handlers_see_both_edges) {
   RoutingScene scene{*services};
   std::vector<bool> states;
   scene.bindHold("MOVE",
                  [&states](bool const held) { states.push_back(held); });

   press(scene, "MOVE");
   release(scene, "MOVE");

   std::vector<bool> const expected{true, false};
   BOOST_TEST(states == expected, boost::test_tools::per_element());
}

BOOST_AUTO_TEST_CASE(unbound_actions_are_ignored) {
   RoutingScene scene{*services};

   press(scene, "NOTHING_IS_BOUND_TO_THIS");

   BOOST_CHECK(router.loaded.empty());
   BOOST_CHECK(!router.quitCalled);
}

BOOST_AUTO_TEST_CASE(registered_keys_map_onto_action_names) {
   RoutingScene scene{*services};
   scene.registerAction(SDLK_W, "UP");
   scene.registerAction(SDLK_UP, "UP");

   BOOST_REQUIRE(scene.getActionMap().contains(SDLK_W));
   BOOST_REQUIRE(scene.getActionMap().contains(SDLK_UP));
   BOOST_CHECK_EQUAL(scene.getActionMap().at(SDLK_W), "UP");
   BOOST_CHECK_EQUAL(scene.getActionMap().at(SDLK_UP), "UP");
}

BOOST_AUTO_TEST_SUITE_END()
