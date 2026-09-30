#include <SDL3/SDL.h>

#include <Configuration/AudioIds.hpp>
#include <MatchScene/MatchScene.hpp>
#include <MenuScene/MenuScene.hpp>
#include <ResultScene/ResultScene.hpp>

using namespace YerbEngine;

namespace {
constexpr SDL_Color TEXT_COLOR = {.r = 235, .g = 235, .b = 235, .a = 255};
constexpr SDL_Color HINT_COLOR = {.r = 120, .g = 120, .b = 120, .a = 255};
}  // namespace

ResultScene::ResultScene(PongServices const& services, MatchMode const mode,
                         int const leftScore, int const rightScore)
    : PongScene(services),
      m_mode(mode),
      m_leftScore(leftScore),
      m_rightScore(rightScore) {
   registerAction(SDLK_RETURN, "REMATCH");
   registerAction(SDLK_BACKSPACE, "GO_BACK");

   auto const chooseExit = [this](bool const rematch) {
      m_services.audio.queueSample(PongAudio::SAMPLE_MENU_SELECT,
                                   PriorityLevel::BACKGROUND);
      m_rematchSelected = rematch;
      m_endTriggered = true;
   };

   bindPress("REMATCH", [chooseExit](Action&) { chooseExit(true); });
   bindPress("GO_BACK", [chooseExit](Action&) { chooseExit(false); });
}

void ResultScene::update() {
   sRender();
   sAudio();

   if (m_endTriggered) {
      onEnd();
   }
}

void ResultScene::onEnd() {
   if (m_rematchSelected) {
      m_services.router.LoadScene("Match",
                                  std::make_shared<MatchScene>(m_pong, m_mode));
      return;
   }

   m_services.router.LoadScene("Menu", std::make_shared<MenuScene>(m_pong));
}

void ResultScene::renderText() const {
   SDL_Renderer* renderer = m_services.video.getRenderer();
   TTF_Font* fontLg = m_services.fonts.getFontLg();
   TTF_Font* fontMd = m_services.fonts.getFontMd();
   TTF_Font* fontSm = m_services.fonts.getFontSm();
   Vec2 const windowSize = m_services.video.getWindowSize();

   if (fontLg == nullptr || fontMd == nullptr || fontSm == nullptr) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                   "Failed to load fonts as they are null.");
      return;
   }

   bool const leftWon = m_leftScore > m_rightScore;

   std::string const winnerText =
       m_mode == MatchMode::SinglePlayer
           ? (leftWon ? "You win!" : "CPU wins!")
           : (leftWon ? "Left player wins!" : "Right player wins!");

   float const centerX = windowSize.x() / 2.0f;

   TextHelpers::renderLineOfText(renderer, fontMd, winnerText, TEXT_COLOR,
                                 Vec2{centerX, windowSize.y() * 0.22f},
                                 TextHelpers::TextAlign::Center);

   std::string const scoreText =
       std::to_string(m_leftScore) + " - " + std::to_string(m_rightScore);
   TextHelpers::renderLineOfText(renderer, fontLg, scoreText, TEXT_COLOR,
                                 Vec2{centerX, windowSize.y() * 0.38f},
                                 TextHelpers::TextAlign::Center);

   TextHelpers::renderLineOfText(
       renderer, fontSm, "Enter for a rematch, Backspace for the menu",
       HINT_COLOR, Vec2{centerX, windowSize.y() - 60.0f},
       TextHelpers::TextAlign::Center);
}

void ResultScene::sRender() {
   VideoManager& videoManager = m_services.video;

   RenderSystem::clear(videoManager);
   renderText();
   RenderSystem::present(videoManager);
}
