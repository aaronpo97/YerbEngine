#pragma once
#include <DemoScene.hpp>
#include <YerbEngine.hpp>

class HowToPlayScene final : public DemoScene {
   void renderText() const;

  public:
   explicit HowToPlayScene(DemoServices const& services);

   void update() override;
   void onEnd() override;
   void sRender() override;
   void sAudio() override;
};
