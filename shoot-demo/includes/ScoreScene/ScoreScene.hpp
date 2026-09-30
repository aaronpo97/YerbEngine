#pragma once

#include <DemoScene.hpp>
#include <YerbEngine.hpp>

class ScoreScene final : public DemoScene {
   unsigned int m_score;
   int m_selectedIndex = 0;

   void renderText() const;
   void moveSelection(int delta);

  public:
   ScoreScene(DemoServices const& services, int score);

   void update() override;
   void onEnd() override;
   void sRender() override;
   void sAudio() override;
};
