#include <YerbEngine/YerbEngine.hpp>
#include "shoot-demo/includes/HowToPlayScene/HowToPlayScene.hpp"
#include "shoot-demo/includes/MenuScene/MenuScene.hpp"
#include <SDL.h>

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
    m_gameEngine->loadScene("Menu", std::make_shared<MenuScene>(m_gameEngine));
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
    Vec2 const        titlePos  = {100, 100};
    TextHelpers::renderLineOfText(renderer, fontLg, titleText, textColor,
                                  titlePos);

    // Movement instructions
    std::string const controlsText = "Controls";
    Vec2 const        controlsPos  = titlePos + Vec2{0, 80};
    TextHelpers::renderLineOfText(renderer, fontMd, controlsText, textColor,
                                  controlsPos);

    std::string const wText = "W: Move Up";
    Vec2 const        wPos  = controlsPos + Vec2{0, 80};
    TextHelpers::renderLineOfText(renderer, fontSm, wText, textColor, wPos);

    std::string const sText = "S: Move Down";
    Vec2 const        sPos  = wPos + Vec2{0, 40};
    TextHelpers::renderLineOfText(renderer, fontSm, sText, textColor, sPos);

    std::string const aText = "A: Move Left";
    Vec2 const        aPos  = sPos + Vec2{0, 40};
    TextHelpers::renderLineOfText(renderer, fontSm, aText, textColor, aPos);

    std::string const dText = "D: Move Right";
    Vec2 const        dPos  = aPos + Vec2{0, 40};
    TextHelpers::renderLineOfText(renderer, fontSm, dText, textColor, dPos);

    std::string const enterText = "Enter: Select";
    Vec2 const        enterPos  = dPos + Vec2{0, 80};
    TextHelpers::renderLineOfText(renderer, fontSm, enterText, textColor,
                                  enterPos);

    std::string const backText = "Back: Backspace";
    Vec2 const        backPos  = enterPos + Vec2{0, 40};
    TextHelpers::renderLineOfText(renderer, fontSm, backText, textColor,
                                  backPos);

    std::string const shootText = "Mouse Click: Shoot";
    Vec2 const        shootPos  = backPos + Vec2{0, 40};
    TextHelpers::renderLineOfText(renderer, fontSm, shootText, textColor,
                                  shootPos);

    // Objectives text

    std::string const objectivesText = "Objectives";
    Vec2 const        objectivesPos  = controlsPos + Vec2{350, 0};
    TextHelpers::renderLineOfText(renderer, fontMd, objectivesText, textColor,
                                  objectivesPos);

    std::string const objective1Text =
        "1. Collect the yellow squares to increase your score.";
    Vec2 const objective1Pos = objectivesPos + Vec2{0, 80};
    TextHelpers::renderLineOfText(renderer, fontSm, objective1Text, textColor,
                                  objective1Pos);

    std::string const objective2Text =
        "2. Avoid the red squares as they will decrease your lives.";
    Vec2 const objective2Pos = objective1Pos + Vec2{0, 40};
    TextHelpers::renderLineOfText(renderer, fontSm, objective2Text, textColor,
                                  objective2Pos);

    std::string const objective3Text =
        "3. Collect the green squares to gain a speed boost.";
    Vec2 const objective3Pos = objective2Pos + Vec2{0, 40};
    TextHelpers::renderLineOfText(renderer, fontSm, objective3Text, textColor,
                                  objective3Pos);

    std::string const objective4Text =
        "4. Avoid purple squares as they will slow you down.";
    Vec2 const objective4Pos = objective3Pos + Vec2{0, 40};
    TextHelpers::renderLineOfText(renderer, fontSm, objective4Text, textColor,
                                  objective4Pos);

    std::string const objective5Text =
        "5. Shoot down red squares to increase your score.";
    Vec2 const objective5Pos = objective4Pos + Vec2{0, 40};
    TextHelpers::renderLineOfText(renderer, fontSm, objective5Text, textColor,
                                  objective5Pos);

    std::string const objective6Text =
        "6. Avoid shooting any squares you want to collect.";
    Vec2 const objective6Pos = objective5Pos + Vec2{0, 40};
    TextHelpers::renderLineOfText(renderer, fontSm, objective6Text, textColor,
                                  objective6Pos);

    // How to exit text
    std::string const exitText = "Press Backspace to go back to the main menu.";
    Vec2 const        windowSize =
        m_gameEngine->getConfigManager().getGameConfig().windowSize;
    Vec2 const exitPos = {100, windowSize.y - 50};
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
                                     AudioSamplePriority::BACKGROUND);
        m_endTriggered = true;
    }

    if (action.getName() == "GO_BACK") {
        audioSampleQueue.queueSample(AudioSample::MENU_SELECT,
                                     AudioSamplePriority::BACKGROUND);
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