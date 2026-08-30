#include <SDL3/SDL.h>

#include <Configuration/AudioIds.hpp>
#include <Helpers/PongTextHelpers.hpp>
#include <MatchScene/MatchScene.hpp>
#include <MenuScene/MenuScene.hpp>
#include <ResultScene/ResultScene.hpp>

namespace {
constexpr SDL_Color TEXT_COLOR = {.r = 235, .g = 235, .b = 235, .a = 255};
constexpr SDL_Color HINT_COLOR = {.r = 120, .g = 120, .b = 120, .a = 255};
}  // namespace

ResultScene::ResultScene(GameEngine* gameEngine, MatchMode const mode,
                         int const leftScore, int const rightScore)
    : Scene(gameEngine),
      m_mode(mode),
      m_leftScore(leftScore),
      m_rightScore(rightScore) {
   registerAction(SDLK_RETURN, "REMATCH");
   registerAction(SDLK_BACKSPACE, "GO_BACK");
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
      m_gameEngine->LoadScene(
          "Match", std::make_shared<MatchScene>(m_gameEngine, m_mode));
      return;
   }

   m_gameEngine->LoadScene("Menu", std::make_shared<MenuScene>(m_gameEngine));
}

void ResultScene::renderText() const {
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

   bool const leftWon = m_leftScore > m_rightScore;

   std::string const winnerText =
       m_mode == MatchMode::SinglePlayer
           ? (leftWon ? "You win!" : "CPU wins!")
           : (leftWon ? "Left player wins!" : "Right player wins!");

   float const centerX = windowSize.x() / 2.0f;

   PongTextHelpers::renderCenteredText(renderer, fontMd, winnerText, TEXT_COLOR,
                                       Vec2{centerX, windowSize.y() * 0.22f});

   std::string const scoreText =
       std::to_string(m_leftScore) + " - " + std::to_string(m_rightScore);
   PongTextHelpers::renderCenteredText(renderer, fontLg, scoreText, TEXT_COLOR,
                                       Vec2{centerX, windowSize.y() * 0.38f});

   PongTextHelpers::renderCenteredText(
       renderer, fontSm, "Enter for a rematch, Backspace for the menu",
       HINT_COLOR, Vec2{centerX, windowSize.y() - 60.0f});
}

void ResultScene::sRender() {
   SDL_Renderer* renderer = m_gameEngine->getVideoManager().getRenderer();
   SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
   SDL_RenderClear(renderer);
   renderText();
   SDL_RenderPresent(renderer);
}

void ResultScene::sDoAction(Action& action) {
   if (action.getState() == ActionState::END) {
      return;
   }

   std::string const& actionName = action.getName();
   if (actionName != "REMATCH" && actionName != "GO_BACK") {
      return;
   }

   m_gameEngine->getAudioSampleBuffer().queueSample(
       PongAudio::SAMPLE_MENU_SELECT, PriorityLevel::BACKGROUND);

   m_rematchSelected = actionName == "REMATCH";
   m_endTriggered = true;
}

void ResultScene::sAudio() { m_gameEngine->getAudioSampleBuffer().update(); }

void ResultScene::onSceneWindowResize() {}
