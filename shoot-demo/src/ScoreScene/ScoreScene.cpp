#include <YerbEngine/YerbEngine.hpp>
#include "shoot-demo/includes/ScoreScene/ScoreScene.hpp"
#include "shoot-demo/includes/MainScene/MainScene.hpp"
#include "shoot-demo/includes/MenuScene/MenuScene.hpp"
#include <SDL.h>

ScoreScene::ScoreScene(GameEngine *gameEngine, int const score)
    : Scene(gameEngine), m_score(score) {

    registerAction(SDLK_RETURN, "SELECT");
    registerAction(SDLK_w, "UP");
    registerAction(SDLK_s, "DOWN");
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
        m_gameEngine->loadScene("Main",
                                std::make_shared<MainScene>(m_gameEngine));
    } else if (m_selectedIndex == 1) {
        m_gameEngine->loadScene("Menu",
                                std::make_shared<MenuScene>(m_gameEngine));
    }
}

void ScoreScene::sRender() {
    SDL_Renderer *renderer = m_gameEngine->getVideoManager().getRenderer();
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    renderText();
    SDL_RenderPresent(renderer);
}

void ScoreScene::renderText() const {
    SDL_Renderer *renderer = m_gameEngine->getVideoManager().getRenderer();
    TTF_Font     *fontLg   = m_gameEngine->getFontManager().getFontLg();
    TTF_Font     *fontMd   = m_gameEngine->getFontManager().getFontMd();

    constexpr SDL_Color gameOverColor = {255, 0, 0, 255};
    constexpr SDL_Color textColor     = {255, 255, 255, 255};
    constexpr SDL_Color selectedColor = {0, 255, 0, 255};

    bool const fontsLoaded = fontLg != nullptr && fontMd != nullptr;
    if (!fontsLoaded) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                     "Failed to load fonts as they are null.");
        return;
    }

    std::string const gameOverText = "Game Over!";
    Vec2 const        gameOverPos  = {100, 300};
    TextHelpers::renderLineOfText(renderer, fontLg, gameOverText, gameOverColor,
                                  gameOverPos);

    std::string const scoreText = "Score: " + std::to_string(m_score);
    Vec2 const        scorePos  = {100, 350};
    TextHelpers::renderLineOfText(renderer, fontMd, scoreText, textColor,
                                  scorePos);

    float const bottomOfScreen =
        m_gameEngine->getConfigManager().getGameConfig().windowSize.y;

    std::string const playAgainText = "Play Again";
    Vec2 const        playAgainPos  = {100, bottomOfScreen - 200};
    SDL_Color const   playAgainColor =
        m_selectedIndex == 0 ? selectedColor : textColor;
    TextHelpers::renderLineOfText(renderer, fontMd, playAgainText,
                                  playAgainColor, playAgainPos);

    std::string const mainMenuText = "Main Menu";
    Vec2 const        mainMenuPos  = {100, bottomOfScreen - 150};
    SDL_Color const   mainMenuColor =
        m_selectedIndex == 1 ? selectedColor : textColor;
    TextHelpers::renderLineOfText(renderer, fontMd, mainMenuText, mainMenuColor,
                                  mainMenuPos);
}

void ScoreScene::sDoAction(Action &action) {

    AudioSampleQueue &audioSampleQueue = m_gameEngine->getAudioSampleQueue();

    if (action.getState() == ActionState::END) {
        return;
    }

    if (action.getName() == "SELECT") {
        audioSampleQueue.queueSample(AudioSample::MENU_SELECT,
                                     AudioSamplePriority::BACKGROUND);
        m_endTriggered = true;
        return;
    }

    // UP takes precedence over DOWN if both are pressed
    if (action.getName() == "UP") {
        audioSampleQueue.queueSample(AudioSample::MENU_MOVE,
                                     AudioSamplePriority::BACKGROUND);
        m_selectedIndex > 0 ? m_selectedIndex -= 1 : m_selectedIndex = 1;
        return;
    }

    if (action.getName() == "DOWN") {
        audioSampleQueue.queueSample(AudioSample::MENU_MOVE,
                                     AudioSamplePriority::BACKGROUND);
        m_selectedIndex < 1 ? m_selectedIndex += 1 : m_selectedIndex = 0;
    }
}

void ScoreScene::sAudio() {
    AudioManager     &audioManager     = m_gameEngine->getAudioManager();
    AudioSampleQueue &audioSampleQueue = m_gameEngine->getAudioSampleQueue();
    if (audioManager.getCurrentAudioTrack() != AudioTrack::MAIN_MENU) {
        m_gameEngine->getAudioManager().playTrack(AudioTrack::MAIN_MENU, -1);
    }

    audioSampleQueue.update();
}