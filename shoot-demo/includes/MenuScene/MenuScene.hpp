#pragma once
#include <YerbEngine.hpp>
using namespace YerbEngine;
#include <map>
#include <string>

class MenuScene final : public Scene {
  private:
   void renderText() const;
   void moveSelection(int delta);
   int m_selectedIndex = 0;

  public:
   explicit MenuScene(GameEngine* gameEngine);
   void update() override;
   void onEnd() override;
   void sRender() override;
   void sAudio() override;
};
