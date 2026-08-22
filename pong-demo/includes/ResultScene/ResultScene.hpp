#pragma once

#include <MatchScene/MatchScene.hpp>
#include <SDL.h>
#include <YerbEngine.hpp>
using namespace YerbEngine;

class ResultScene final : public Scene {
    MatchMode m_mode;
    int       m_leftScore;
    int       m_rightScore;
    bool      m_rematchSelected = false;

    void renderText() const;

  public:
    ResultScene(GameEngine *gameEngine,
                MatchMode   mode,
                int         leftScore,
                int         rightScore);

    void update() override;
    void onEnd() override;
    void sRender() override;
    void sDoAction(Action &action) override;
    void sAudio() override;
    void onSceneWindowResize() override;
};
