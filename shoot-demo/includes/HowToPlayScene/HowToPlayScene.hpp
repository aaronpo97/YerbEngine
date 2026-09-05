#pragma once
#include <YerbEngine.hpp>
using namespace YerbEngine;

class HowToPlayScene final : public Scene {
  private:
   void renderText() const;

  public:
   explicit HowToPlayScene(GameEngine* gameEngine);

   void update() override;
   void onEnd() override;
   void sRender() override;
   void sAudio() override;
};
