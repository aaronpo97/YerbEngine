#include <HowToPlayScene/HowToPlayScene.hpp>
#include <MenuScene/MenuScene.hpp>
#include <SDL.h>
#include <YerbEngine.hpp>
using namespace YerbEngine;

HowToPlayScene::HowToPlayScene(GameEngine *gameEngine) : Scene(gameEngine) {
    registerAction(SDLK_RETURN, "SELECT");
    registerAction(SDLK_BACKSPACE, "GO_BACK");
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

void HowToPlayScene::sRender() {
    SDL_Renderer *renderer = m_gameEngine->getVideoManager().getRenderer();
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    renderText();
    SDL_RenderPresent(renderer);
}

void HowToPlayScene::renderText() const {
    SDL_Renderer *renderer = m_gameEngine->getVideoManager().getRenderer();
    TTF_Font     *fontSm   = m_gameEngine->getFontManager().getFontSm();
    TTF_Font     *fontMd   = m_gameEngine->getFontManager().getFontMd();
    TTF_Font     *fontLg   = m_gameEngine->getFontManager().getFontLg();
    constexpr SDL_Color textColor = {.r = 255, .g = 255, .b = 255, .a = 255};

    bool const fontsLoaded =
        fontSm != nullptr && fontMd != nullptr && fontLg != nullptr;

    if (!fontsLoaded) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                     "Failed to load fonts as they are null.");
        return;
    }

    // Instructions text
    std::string const titleText = "How to Play";
    constexpr Vec2    titlePos{100, 100};
    TextHelpers::renderLineOfText(renderer, fontLg, titleText, textColor,
                                  titlePos);

    // Movement instructions
    std::string const controlsText = "Controls";
    constexpr Vec2    controlsPos(titlePos + Vec2{0, 80});
    TextHelpers::renderLineOfText(renderer, fontMd, controlsText, textColor,
                                  controlsPos);

    std::string const wText = "W: Move Up";
    constexpr Vec2    wPos(controlsPos + Vec2{0, 80});
    TextHelpers::renderLineOfText(renderer, fontSm, wText, textColor, wPos);

    std::string const sText = "S: Move Down";
    constexpr Vec2    sPos(wPos + Vec2{0, 40});
    TextHelpers::renderLineOfText(renderer, fontSm, sText, textColor, sPos);

    std::string const aText = "A: Move Left";
    constexpr Vec2    aPos(sPos + Vec2{0, 40});
    TextHelpers::renderLineOfText(renderer, fontSm, aText, textColor, aPos);

    std::string const dText = "D: Move Right";
    constexpr Vec2    dPos(aPos + Vec2{0, 40});
    TextHelpers::renderLineOfText(renderer, fontSm, dText, textColor, dPos);

    std::string const enterText = "Enter: Select";
    constexpr Vec2    enterPos(dPos + Vec2{0, 80});
    TextHelpers::renderLineOfText(renderer, fontSm, enterText, textColor,
                                  enterPos);

    std::string const backText = "Back: Backspace";
    constexpr Vec2    backPos(enterPos + Vec2{0, 40});
    TextHelpers::renderLineOfText(renderer, fontSm, backText, textColor,
                                  backPos);

    std::string const shootText = "Mouse Click: Shoot";
    constexpr Vec2    shootPos(backPos + Vec2{0, 40});
    TextHelpers::renderLineOfText(renderer, fontSm, shootText, textColor,
                                  shootPos);

    // Objectives text

    std::string const objectivesText = "Objectives";
    constexpr Vec2    objectivesPos(controlsPos + Vec2{350, 0});
    TextHelpers::renderLineOfText(renderer, fontMd, objectivesText, textColor,
                                  objectivesPos);

    std::string const objective1Text =
        "1. Collect the yellow squares to increase your score.";
    constexpr Vec2 objective1Pos(objectivesPos + Vec2{0, 80});
    TextHelpers::renderLineOfText(renderer, fontSm, objective1Text, textColor,
                                  objective1Pos);

    std::string const objective2Text =
        "2. Avoid the red squares as they will decrease your lives.";
    constexpr Vec2 objective2Pos(objective1Pos + Vec2{0, 40});
    TextHelpers::renderLineOfText(renderer, fontSm, objective2Text, textColor,
                                  objective2Pos);

    std::string const objective3Text =
        "3. Collect the green squares to gain a speed boost.";
    constexpr Vec2 objective3Pos(objective2Pos + Vec2{0, 40});
    TextHelpers::renderLineOfText(renderer, fontSm, objective3Text, textColor,
                                  objective3Pos);

    std::string const objective4Text =
        "4. Avoid purple squares as they will slow you down.";
    constexpr Vec2 objective4Pos(objective3Pos + Vec2{0, 40});
    TextHelpers::renderLineOfText(renderer, fontSm, objective4Text, textColor,
                                  objective4Pos);

    std::string const objective5Text =
        "5. Shoot down red squares to increase your score.";
    constexpr Vec2 objective5Pos(objective4Pos + Vec2{0, 40});
    TextHelpers::renderLineOfText(renderer, fontSm, objective5Text, textColor,
                                  objective5Pos);

    std::string const objective6Text =
        "6. Avoid shooting any squares you want to collect.";
    constexpr Vec2 objective6Pos(objective5Pos + Vec2{0, 40});
    TextHelpers::renderLineOfText(renderer, fontSm, objective6Text, textColor,
                                  objective6Pos);

    // How to exit text
    std::string const exitText = "Press Backspace to go back to the main menu.";
    Vec2 const        windowSize =
        m_gameEngine->GetConfigManager().getGameConfig().windowSize;
    Vec2 const exitPos{100, windowSize.y() - 50};
    TextHelpers::renderLineOfText(renderer, fontSm, exitText, textColor,
                                  exitPos);
}

void HowToPlayScene::sDoAction(Action &action) {
    AudioSampleQueue &audioSampleQueue = m_gameEngine->getAudioSampleQueue();
    if (action.getState() == ActionState::END) {
        return;
    }

    if (action.getName() == "SELECT") {
        audioSampleQueue.queueSample(AudioSample::MENU_SELECT,
                                     PriorityLevel::BACKGROUND);
        m_endTriggered = true;
    }

    if (action.getName() == "GO_BACK") {
        audioSampleQueue.queueSample(AudioSample::MENU_SELECT,
                                     PriorityLevel::BACKGROUND);
        m_endTriggered = true;
    }
}

void HowToPlayScene::sAudio() {
    AudioManager     &audioManager     = m_gameEngine->getAudioManager();
    AudioSampleQueue &audioSampleQueue = m_gameEngine->getAudioSampleQueue();
    if (audioManager.getCurrentAudioTrack() != AudioTrack::MAIN_MENU) {
        audioManager.playTrack(AudioTrack::MAIN_MENU, -1);
    }

    audioSampleQueue.update();
}