#pragma once

#include <SDL3/SDL.h>

#include <YerbEngine.hpp>
using namespace YerbEngine;

class MenuScene final : public Scene {
   int m_selectedIndex = 0;

   void renderText() const;
   void moveSelection(int delta);

  public:
   explicit MenuScene(GameEngine* gameEngine);

   void update() override;
   void onEnd() override;
   void sRender() override;
};
