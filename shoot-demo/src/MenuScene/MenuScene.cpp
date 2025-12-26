#include <HowToPlayScene/HowToPlayScene.hpp>
#include <MainScene/MainScene.hpp>
#include <MenuScene/MenuScene.hpp>
#include <YerbEngine.hpp>

#include <SDL.h>

MenuScene::MenuScene(GameEngine *gameEngine) : Scene(gameEngine) {
    m_selectedIndex = 0;
    registerAction(SDLK_RETURN, "SELECT");
    registerAction(SDLK_w, "UP");
    registerAction(SDLK_s, "DOWN");
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
        m_gameEngine->LoadScene("Main",
                                std::make_shared<MainScene>(m_gameEngine));
        break;
    case 1:
        m_gameEngine->LoadScene("HowToPlay",
                                std::make_shared<HowToPlayScene>(m_gameEngine));
        break;
    case 2:
        m_gameEngine->quit();
        break;
    default:;
    }
}

void MenuScene::sRender() {
    SDL_Renderer *renderer = m_gameEngine->getVideoManager().getRenderer();
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    renderText();
    SDL_RenderPresent(renderer);
}

void MenuScene::renderText() const {
    SDL_Renderer *renderer      = m_gameEngine->getVideoManager().getRenderer();
    TTF_Font     *fontLg        = m_gameEngine->getFontManager().getFontLg();
    TTF_Font     *fontMd        = m_gameEngine->getFontManager().getFontMd();
    TTF_Font     *fontSm        = m_gameEngine->getFontManager().getFontSm();
    SDL_Color     textColor     = {255, 255, 255, 255};
    SDL_Color     selectedColor = {0, 255, 0, 255};

    if (fontLg == nullptr || fontMd == nullptr || fontSm == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                     "Failed to load fonts as they are null.");
        return;
    }

    std::string const titleText =
        m_gameEngine->GetConfig().getGameConfig().windowTitle;
    Vec2 const titlePos{100, 100};
    TextHelpers::renderLineOfText(renderer, fontLg, titleText, textColor,
                                  titlePos);

    std::string const playText = "Play";
    Vec2 const        playPos  = titlePos + Vec2{0, 100};
    SDL_Color const   playColor =
        m_selectedIndex == 0 ? selectedColor : textColor;
    TextHelpers::renderLineOfText(renderer, fontMd, playText, playColor,
                                  playPos);

    std::string const instructionsText = "How to Play";
    Vec2 const        instructionsPos  = playPos + Vec2{0, 50};
    SDL_Color const   instructionsColor =
        m_selectedIndex == 1 ? selectedColor : textColor;
    TextHelpers::renderLineOfText(renderer, fontMd, instructionsText,
                                  instructionsColor, instructionsPos);

#ifndef __EMSCRIPTEN__
    const std::string quitText = "Quit";
    Vec2 const        quitPos  = instructionsPos + Vec2{0, 50};
    SDL_Color const   quitColor =
        m_selectedIndex == 2 ? selectedColor : textColor;
    TextHelpers::renderLineOfText(renderer, fontMd, quitText, quitColor,
                                  quitPos);
#endif

    const std::string controlsText = "W/S to move up/down, Enter to select";
    // bottom right corner
    Vec2 const controlsPos{
        100, m_gameEngine->getVideoManager().getWindowSize().y() - 50};
    TextHelpers::renderLineOfText(renderer, fontSm, controlsText, textColor,
                                  controlsPos);
}

void MenuScene::sDoAction(Action &action) {

// Emscripten build does not have the quit option (option 3)
#ifdef __EMSCRIPTEN__
    constexpr int MAX_MENU_ITEMS = 2;
#else
    constexpr int MAX_MENU_ITEMS = 3;
#endif

    AudioSampleBuffer &audioSampleBuffer = m_gameEngine->getAudioSampleBuffer();
    if (action.getState() == ActionState::END) {
        return;
    }

    if (action.getName() == "SELECT") {
        audioSampleBuffer.queueSample(AudioSample::MENU_SELECT,
                                      PriorityLevel::BACKGROUND);
        m_endTriggered = true;
        return;
    }

    // UP takes precedence over DOWN if both are pressed
    if (action.getName() == "UP") {
        audioSampleBuffer.queueSample(AudioSample::MENU_MOVE,
                                      PriorityLevel::BACKGROUND);
        m_selectedIndex > 0 ? m_selectedIndex -= 1
                            : m_selectedIndex = MAX_MENU_ITEMS - 1;
        return;
    }

    if (action.getName() == "DOWN") {
        audioSampleBuffer.queueSample(AudioSample::MENU_MOVE,
                                      PriorityLevel::BACKGROUND);
        m_selectedIndex < MAX_MENU_ITEMS - 1 ? m_selectedIndex += 1
                                             : m_selectedIndex = 0;
    }
}

void MenuScene::sAudio() {
    AudioManager      &audioManager      = m_gameEngine->getAudioManager();
    AudioSampleBuffer &audioSampleBuffer = m_gameEngine->getAudioSampleBuffer();

    // TEMPORARY FOR NOW
    audioManager.muteTracks();

    if (audioManager.getCurrentAudioTrack() != AudioTrack::MAIN_MENU) {
        m_gameEngine->getAudioManager().playTrack(AudioTrack::MAIN_MENU, -1);
    }
    audioSampleBuffer.update();
}
