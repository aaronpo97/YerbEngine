#include <SDL3/SDL.h>

#include <Configuration/AudioIds.hpp>
#include <MainScene/MainScene.hpp>
#include <MenuScene/MenuScene.hpp>
#include <ScoreScene/ScoreScene.hpp>
#include <YerbEngine.hpp>

using namespace YerbEngine;

ScoreScene::ScoreScene(DemoServices const& services, int const score)
    : DemoScene(services), m_score(score) {
   registerAction(SDLK_RETURN, "SELECT");
   registerAction(SDLK_W, "UP");
   registerAction(SDLK_S, "DOWN");

   bindPress("SELECT", [this](Action&) {
      m_services.audio.queueSample(DemoAudio::SAMPLE_MENU_SELECT,
                                   PriorityLevel::BACKGROUND);
      m_endTriggered = true;
   });
   bindPress("UP", [this](Action&) { moveSelection(-1); });
   bindPress("DOWN", [this](Action&) { moveSelection(1); });
}

void ScoreScene::moveSelection(int const delta) {
   m_services.audio.queueSample(DemoAudio::SAMPLE_MENU_MOVE,
                                PriorityLevel::BACKGROUND);

   constexpr int ITEM_COUNT = 2;
   m_selectedIndex = (m_selectedIndex + delta + ITEM_COUNT) % ITEM_COUNT;
}

void ScoreScene::update() {
   sRender();
   sAudio();

   if (m_endTriggered) {
      onEnd();
   }
}

void ScoreScene::onEnd() {
   if (m_selectedIndex == 0) {
      m_services.router.LoadScene("Main", std::make_shared<MainScene>(m_demo));
   } else if (m_selectedIndex == 1) {
      m_services.router.LoadScene("Menu", std::make_shared<MenuScene>(m_demo));
   }
}

void ScoreScene::sRender() {
   VideoManager& videoManager = m_services.video;

   RenderSystem::clear(videoManager);
   renderText();
   RenderSystem::present(videoManager);
}

void ScoreScene::renderText() const {
   SDL_Renderer* renderer = m_services.video.getRenderer();
   TTF_Font* fontLg = m_services.fonts.getFontLg();
   TTF_Font* fontMd = m_services.fonts.getFontMd();

   constexpr SDL_Color gameOverColor = {255, 0, 0, 255};
   constexpr SDL_Color textColor = {255, 255, 255, 255};
   constexpr SDL_Color selectedColor = {0, 255, 0, 255};

   bool const fontsLoaded = fontLg != nullptr && fontMd != nullptr;
   if (!fontsLoaded) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                   "Failed to load fonts as they are null.");
      return;
   }

   std::string const gameOverText = "Game Over!";
   Vec2 const gameOverPos{100, 300};
   TextHelpers::renderLineOfText(renderer, fontLg, gameOverText, gameOverColor,
                                 gameOverPos);

   std::string const scoreText = "Score: " + std::to_string(m_score);
   Vec2 const scorePos{100, 350};
   TextHelpers::renderLineOfText(renderer, fontMd, scoreText, textColor,
                                 scorePos);

   float const bottomOfScreen = m_services.video.getWindowSize().y();

   std::string const playAgainText = "Play Again";
   Vec2 const playAgainPos{100, bottomOfScreen - 200};
   SDL_Color const playAgainColor =
       m_selectedIndex == 0 ? selectedColor : textColor;
   TextHelpers::renderLineOfText(renderer, fontMd, playAgainText,
                                 playAgainColor, playAgainPos);

   std::string const mainMenuText = "Main Menu";
   Vec2 const mainMenuPos{100, bottomOfScreen - 150};
   SDL_Color const mainMenuColor =
       m_selectedIndex == 1 ? selectedColor : textColor;
   TextHelpers::renderLineOfText(renderer, fontMd, mainMenuText, mainMenuColor,
                                 mainMenuPos);
}

void ScoreScene::sAudio() {
   AudioManager& audioManager = m_services.mixer;
   if (audioManager.getCurrentAudioTrack() !=
       std::string(DemoAudio::TRACK_MAIN_MENU)) {
      audioManager.playTrack(DemoAudio::TRACK_MAIN_MENU, -1);
   }

   Scene::sAudio();
}
