#pragma once

#include <SDL.h>

#include <YerbEngine.hpp>
using namespace YerbEngine;

class HowToPlayScene final : public Scene {
   void renderText() const;

  public:
   explicit HowToPlayScene(GameEngine* gameEngine);

   void update() override;
   void onEnd() override;
   void sRender() override;
   void sDoAction(Action& action) override;
   void sAudio() override;
   void onSceneWindowResize() override;
};
