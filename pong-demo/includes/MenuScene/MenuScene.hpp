#pragma once

#include <SDL3/SDL.h>

#include <PongScene.hpp>
#include <YerbEngine.hpp>

class MenuScene final : public PongScene {
   int m_selectedIndex = 0;

   void renderText() const;
   void moveSelection(int delta);

  public:
   explicit MenuScene(PongServices const& services);

   void update() override;
   void onEnd() override;
   void sRender() override;
};
