#pragma once

#include <SDL3/SDL.h>

#include <PongScene.hpp>
#include <YerbEngine.hpp>

class HowToPlayScene final : public PongScene {
   void renderText() const;

  public:
   explicit HowToPlayScene(PongServices const& services);

   void update() override;
   void onEnd() override;
   void sRender() override;
};
