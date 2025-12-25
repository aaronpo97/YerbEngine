#pragma once
#include <Configuration/DemoConfigAdapter.hpp>
#include <YerbEngine.hpp>
using namespace YerbEngine;
#include <random>

class MainSceneSpawner {
    std::mt19937 &m_randomGenerator;

  public:
    DemoConfigAdapter &m_config;
    VideoManager      &m_videoManager;
    TextureManager    &m_textureManager;
    EntityManager     &m_entityManager;

  public:
    MainSceneSpawner(std::mt19937      &randomGenerator,
                     DemoConfigAdapter &config,
                     TextureManager    &textureManager,
                     EntityManager     &entityManager,
                     VideoManager      &videoManager);

    std::shared_ptr<Entity> spawnPlayer();

    void spawnEnemy(std::shared_ptr<Entity> const &player);
    void spawnSpeedBoostEntity(std::shared_ptr<Entity> const &player);
    void spawnSlownessEntity(std::shared_ptr<Entity> const &player);
    void spawnWalls();
    void spawnBullets(std::shared_ptr<Entity> const &player,
                      Vec2 const                    &mousePosition);
    void spawnItem(std::shared_ptr<Entity> const &player);
};
