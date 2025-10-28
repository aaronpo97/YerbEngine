#pragma once

#include <YerbEngine/YerbEngine.hpp>
#include "MainSceneSpawner.hpp"
#include <SDL.h>
#include <random>

class MainScene final : public Scene {
  private:
    Uint64                  m_lastNonPlayerEntitySpawnTime = 0;
    Uint64                  m_lastFrameTime                = 0;
    EntityManager           m_entities;
    float                   m_deltaTime = 0;
    bool                    m_paused    = false;
    int                     m_score     = 0;
    int                     m_lives     = 5;
    std::shared_ptr<Entity> m_player;
    Uint64                  m_timeRemaining = 2.5 * 60 * 1000;
    bool                    m_gameOver      = false;
    std::random_device      m_rd;
    std::mt19937            m_randomGenerator     = std::mt19937(m_rd());
    Uint64                  m_lastBulletSpawnTime = 0;
    Uint64                  m_bulletSpawnCooldown = 90;
    MainSceneSpawner        m_spawner;
    void                    renderText() const;

  public:
    explicit MainScene(GameEngine *gameEngine);

    void onSceneWindowResize() override;

    void update() override;
    void onEnd() override;
    void sRender() override;
    void sDoAction(Action &action) override;
    void sAudio() override;

    void sCollision();
    void sMovement();
    void sSpawner();
    void sLifespan();
    void sEffects() const;
    void sTimer();

    int  getScore() const;
    void setScore(int score);
    void decrementLives();

    void setGameOver();
};
