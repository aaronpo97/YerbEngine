#pragma once

#include <SDL3/SDL.h>

#include <MatchScene/MatchScene.hpp>
#include <PongScene.hpp>
#include <YerbEngine.hpp>

class ResultScene final : public PongScene {
   MatchMode m_mode;
   int m_leftScore;
   int m_rightScore;
   bool m_rematchSelected = false;

   void renderText() const;

  public:
   ResultScene(PongServices const& services, MatchMode mode, int leftScore,
               int rightScore);

   void update() override;
   void onEnd() override;
   void sRender() override;
};
