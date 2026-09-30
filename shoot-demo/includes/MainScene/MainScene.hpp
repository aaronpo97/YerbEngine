#pragma once

#include <SDL3/SDL.h>

#include <DemoScene.hpp>
#include <MainScene/MainSceneSpawner.hpp>
#include <YerbEngine.hpp>

class MainScene final : public DemoScene {
   using Entity = YerbEngine::Entity;
   using Vec2 = YerbEngine::Vec2;

   Uint64 m_lastNonPlayerEntitySpawnTime = 0;
   int m_score = 0;
   int m_lives = 5;
   std::shared_ptr<Entity> m_player;
   Uint64 m_timeRemaining = 2.5 * 60 * 1000;
   bool m_gameOver = false;
   Uint64 m_lastBulletSpawnTime = 0;
   Uint64 m_bulletSpawnCooldown = 90;
   MainSceneSpawner m_spawner;

   void renderText() const;
   void bindControls();
   void setPlayerDirection(YerbEngine::Components::CInput::Directions direction,
                           bool pressed);
   void shootAt(Vec2 const& target);

  public:
   explicit MainScene(DemoServices const& services);

   void onSceneWindowResize() override;

   void update() override;
   void onEnd() override;
   void sRender() override;
   void sAudio() override;

   void sCollision();
   void sMovement();
   void sSpawner();
   void sLifespan();
   void sEffects() const;
   void sTimer();

   int getScore() const;
   void setScore(int score);
   void decrementLives();

   void setGameOver();
};
