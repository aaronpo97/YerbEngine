#pragma once

#include <YerbEngine.hpp>
using namespace YerbEngine;

class ScoreScene final : public Scene {
  private:
   unsigned int m_score;
   int m_selectedIndex = 0;

   void renderText() const;
   void moveSelection(int delta);

  public:
   ScoreScene(GameEngine* gameEngine, int score);

   void update() override;
   void onEnd() override;
   void sRender() override;
   void sAudio() override;
};
