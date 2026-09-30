#include <SDL3/SDL.h>

#include <Configuration/AudioIds.hpp>
#include <HowToPlayScene/HowToPlayScene.hpp>
#include <MainScene/MainScene.hpp>
#include <MenuScene/MenuScene.hpp>
#include <YerbEngine.hpp>

using namespace YerbEngine;

namespace {
// The web build drops the Quit entry: the browser owns the window.
#ifdef __EMSCRIPTEN__
constexpr int MENU_ITEM_COUNT = 2;
#else
constexpr int MENU_ITEM_COUNT = 3;
#endif
}  // namespace

MenuScene::MenuScene(DemoServices const& services) : DemoScene(services) {
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

void MenuScene::moveSelection(int const delta) {
   m_services.audio.queueSample(DemoAudio::SAMPLE_MENU_MOVE,
                                PriorityLevel::BACKGROUND);

   m_selectedIndex =
       (m_selectedIndex + delta + MENU_ITEM_COUNT) % MENU_ITEM_COUNT;
}

void MenuScene::update() {
   sRender();
   sAudio();

   if (m_endTriggered) {
      onEnd();
   }
}

void MenuScene::onEnd() {
   switch (m_selectedIndex) {
      case 0:
         m_services.router.LoadScene("Main",
                                     std::make_shared<MainScene>(m_demo));
         break;
      case 1:
         m_services.router.LoadScene("HowToPlay",
                                     std::make_shared<HowToPlayScene>(m_demo));
         break;
      case 2:
         m_services.router.quit();
         break;
      default:;
   }
}

void MenuScene::sRender() {
   VideoManager& videoManager = m_services.video;

   RenderSystem::clear(videoManager);
   renderText();
   RenderSystem::present(videoManager);
}

void MenuScene::renderText() const {
   SDL_Renderer* renderer = m_services.video.getRenderer();
   TTF_Font* fontLg = m_services.fonts.getFontLg();
   TTF_Font* fontMd = m_services.fonts.getFontMd();
   TTF_Font* fontSm = m_services.fonts.getFontSm();
   SDL_Color textColor = {255, 255, 255, 255};
   SDL_Color selectedColor = {0, 255, 0, 255};

   if (fontLg == nullptr || fontMd == nullptr || fontSm == nullptr) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                   "Failed to load fonts as they are null.");
      return;
   }

   std::string const titleText = m_demo.config.getGameConfig().windowTitle;
   Vec2 const titlePos{100, 100};
   TextHelpers::renderLineOfText(renderer, fontLg, titleText, textColor,
                                 titlePos);

   std::string const playText = "Play";
   Vec2 const playPos = titlePos + Vec2{0, 100};
   SDL_Color const playColor = m_selectedIndex == 0 ? selectedColor : textColor;
   TextHelpers::renderLineOfText(renderer, fontMd, playText, playColor,
                                 playPos);

   std::string const instructionsText = "How to Play";
   Vec2 const instructionsPos = playPos + Vec2{0, 50};
   SDL_Color const instructionsColor =
       m_selectedIndex == 1 ? selectedColor : textColor;
   TextHelpers::renderLineOfText(renderer, fontMd, instructionsText,
                                 instructionsColor, instructionsPos);

#ifndef __EMSCRIPTEN__
   const std::string quitText = "Quit";
   Vec2 const quitPos = instructionsPos + Vec2{0, 50};
   SDL_Color const quitColor = m_selectedIndex == 2 ? selectedColor : textColor;
   TextHelpers::renderLineOfText(renderer, fontMd, quitText, quitColor,
                                 quitPos);
#endif

   const std::string controlsText = "W/S to move up/down, Enter to select";
   // bottom right corner
   Vec2 const controlsPos{100, m_services.video.getWindowSize().y() - 50};
   TextHelpers::renderLineOfText(renderer, fontSm, controlsText, textColor,
                                 controlsPos);
}

void MenuScene::sAudio() {
   AudioManager& audioManager = m_services.mixer;

   // TEMPORARY FOR NOW
   audioManager.muteTracks();

   if (audioManager.getCurrentAudioTrack() !=
       std::string(DemoAudio::TRACK_MAIN_MENU)) {
      audioManager.playTrack(DemoAudio::TRACK_MAIN_MENU, -1);
   }

   Scene::sAudio();
}
