#include <SDL3/SDL.h>

#include <Configuration/AudioIds.hpp>
#include <HowToPlayScene/HowToPlayScene.hpp>
#include <MenuScene/MenuScene.hpp>
#include <array>
#include <string_view>

namespace {
constexpr std::array<std::string_view, 6> INSTRUCTIONS = {
    "Left paddle:  W / S",
    "Right paddle: Up / Down arrows",
    "",
    "The ball leaves a paddle at an angle set by where it lands,",
    "and picks up speed with every return.",
    "P pauses, Backspace returns to the menu.",
};

constexpr SDL_Color TEXT_COLOR = {.r = 235, .g = 235, .b = 235, .a = 255};
constexpr SDL_Color HINT_COLOR = {.r = 120, .g = 120, .b = 120, .a = 255};
}  // namespace

HowToPlayScene::HowToPlayScene(GameEngine* gameEngine) : Scene(gameEngine) {
   registerAction(SDLK_RETURN, "GO_BACK");
   registerAction(SDLK_BACKSPACE, "GO_BACK");

   bindPress("GO_BACK", [this](Action&) {
      m_gameEngine->getAudioSampleBuffer().queueSample(
          PongAudio::SAMPLE_MENU_SELECT, PriorityLevel::BACKGROUND);
      m_endTriggered = true;
   });
}

void HowToPlayScene::update() {
   sRender();
   sAudio();

   if (m_endTriggered) {
      onEnd();
   }
}

void HowToPlayScene::onEnd() {
   m_gameEngine->LoadScene("Menu", std::make_shared<MenuScene>(m_gameEngine));
}

void HowToPlayScene::renderText() const {
   SDL_Renderer* renderer = m_gameEngine->getVideoManager().getRenderer();
   TTF_Font* fontMd = m_gameEngine->getFontManager().getFontMd();
   TTF_Font* fontSm = m_gameEngine->getFontManager().getFontSm();
   Vec2 const windowSize = m_gameEngine->getVideoManager().getWindowSize();

   if (fontMd == nullptr || fontSm == nullptr) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                   "Failed to load fonts as they are null.");
      return;
   }

   float const centerX = windowSize.x() / 2.0f;

   TextHelpers::renderLineOfText(renderer, fontMd, "How to Play", TEXT_COLOR,
                                 Vec2{centerX, windowSize.y() * 0.16f},
                                 TextHelpers::TextAlign::Center);

   float const firstLineY = windowSize.y() * 0.36f;
   constexpr float LINE_HEIGHT = 46.0f;

   for (size_t i = 0; i < INSTRUCTIONS.size(); i++) {
      TextHelpers::renderLineOfText(
          renderer, fontSm, std::string(INSTRUCTIONS[i]), TEXT_COLOR,
          Vec2{centerX, firstLineY + static_cast<float>(i) * LINE_HEIGHT},
          TextHelpers::TextAlign::Center);
   }

   TextHelpers::renderLineOfText(
       renderer, fontSm, "Enter or Backspace to go back", HINT_COLOR,
       Vec2{centerX, windowSize.y() - 60.0f}, TextHelpers::TextAlign::Center);
}

void HowToPlayScene::sRender() {
   VideoManager& videoManager = m_gameEngine->getVideoManager();

   RenderSystem::clear(videoManager);
   renderText();
   RenderSystem::present(videoManager);
}
