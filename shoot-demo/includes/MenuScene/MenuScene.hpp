#pragma once
#include <DemoScene.hpp>
#include <YerbEngine.hpp>

class MenuScene final : public DemoScene {
   void renderText() const;
   void moveSelection(int delta);
   int m_selectedIndex = 0;

  public:
   explicit MenuScene(DemoServices const& services);

   void update() override;
   void onEnd() override;
   void sRender() override;
   void sAudio() override;
};
