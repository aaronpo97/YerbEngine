#include <SDL3/SDL.h>

#include <Configuration/AudioIds.hpp>
#include <HowToPlayScene/HowToPlayScene.hpp>
#include <MatchScene/MatchScene.hpp>
#include <MenuScene/MenuScene.hpp>
#include <array>
#include <string_view>

namespace {
#ifdef __EMSCRIPTEN__
constexpr std::array<std::string_view, 3> MENU_ITEMS = {"1 Player", "2 Players",
                                                        "How to Play"};
#else
constexpr std::array<std::string_view, 4> MENU_ITEMS = {"1 Player", "2 Players",
                                                        "How to Play", "Quit"};
#endif

constexpr SDL_Color TEXT_COLOR = {.r = 235, .g = 235, .b = 235, .a = 255};
constexpr SDL_Color SELECTED_COLOR = {.r = 120, .g = 255, .b = 140, .a = 255};
constexpr SDL_Color HINT_COLOR = {.r = 120, .g = 120, .b = 120, .a = 255};
}  // namespace

MenuScene::MenuScene(GameEngine* gameEngine) : Scene(gameEngine) {
   registerAction(SDLK_RETURN, "SELECT");
   registerAction(SDLK_W, "UP");
   registerAction(SDLK_S, "DOWN");
   registerAction(SDLK_UP, "UP");
   registerAction(SDLK_DOWN, "DOWN");

   bindPress("SELECT", [this](Action&) {
      m_gameEngine->getAudioSampleBuffer().queueSample(
          PongAudio::SAMPLE_MENU_SELECT, PriorityLevel::BACKGROUND);
      m_endTriggered = true;
   });
   bindPress("UP", [this](Action&) { moveSelection(-1); });
   bindPress("DOWN", [this](Action&) { moveSelection(1); });
}

void MenuScene::moveSelection(int const delta) {
   m_gameEngine->getAudioSampleBuffer().queueSample(PongAudio::SAMPLE_MENU_MOVE,
                                                    PriorityLevel::BACKGROUND);

   constexpr int ITEM_COUNT = static_cast<int>(MENU_ITEMS.size());
   m_selectedIndex = (m_selectedIndex + delta + ITEM_COUNT) % ITEM_COUNT;
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
         m_gameEngine->LoadScene(
             "Match", std::make_shared<MatchScene>(m_gameEngine,
                                                   MatchMode::SinglePlayer));
         break;
      case 1:
         m_gameEngine->LoadScene(
             "Match",
             std::make_shared<MatchScene>(m_gameEngine, MatchMode::TwoPlayer));
         break;
      case 2:
         m_gameEngine->LoadScene(
             "HowToPlay", std::make_shared<HowToPlayScene>(m_gameEngine));
         break;
      case 3:
         m_gameEngine->quit();
         break;
      default:;
   }
}

void MenuScene::renderText() const {
   SDL_Renderer* renderer = m_gameEngine->getVideoManager().getRenderer();
   TTF_Font* fontLg = m_gameEngine->getFontManager().getFontLg();
   TTF_Font* fontMd = m_gameEngine->getFontManager().getFontMd();
   TTF_Font* fontSm = m_gameEngine->getFontManager().getFontSm();
   Vec2 const windowSize = m_gameEngine->getVideoManager().getWindowSize();

   if (fontLg == nullptr || fontMd == nullptr || fontSm == nullptr) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                   "Failed to load fonts as they are null.");
      return;
   }

   float const centerX = windowSize.x() / 2.0f;

   TextHelpers::renderLineOfText(renderer, fontLg, "PONG", TEXT_COLOR,
                                 Vec2{centerX, windowSize.y() * 0.14f},
                                 TextHelpers::TextAlign::Center);

   float const firstItemY = windowSize.y() * 0.42f;
   constexpr float ITEM_SPACING = 58.0f;

   for (size_t i = 0; i < MENU_ITEMS.size(); i++) {
      bool const isSelected = static_cast<size_t>(m_selectedIndex) == i;
      SDL_Color const color = isSelected ? SELECTED_COLOR : TEXT_COLOR;
      std::string const label = std::string(MENU_ITEMS[i]);

      TextHelpers::renderLineOfText(
          renderer, fontMd, label, color,
          Vec2{centerX, firstItemY + static_cast<float>(i) * ITEM_SPACING},
          TextHelpers::TextAlign::Center);
   }

   TextHelpers::renderLineOfText(
       renderer, fontSm, "W/S to move, Enter to select", HINT_COLOR,
       Vec2{centerX, windowSize.y() - 60.0f}, TextHelpers::TextAlign::Center);
}

void MenuScene::sRender() {
   VideoManager& videoManager = m_gameEngine->getVideoManager();

   RenderSystem::clear(videoManager);
   renderText();
   RenderSystem::present(videoManager);
}
