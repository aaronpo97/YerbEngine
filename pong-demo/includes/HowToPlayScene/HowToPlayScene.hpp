#pragma once

#include <SDL3/SDL.h>

#include <YerbEngine.hpp>
using namespace YerbEngine;

class HowToPlayScene final : public Scene {
   void renderText() const;

  public:
   explicit HowToPlayScene(GameEngine* gameEngine);

   void update() override;
   void onEnd() override;
   void sRender() override;
};
