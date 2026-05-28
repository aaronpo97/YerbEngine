#pragma once
#include <YerbEngine.hpp>
using namespace YerbEngine;
#include <random>

class MainSceneSpawner {
    std::mt19937   &m_randomGenerator;
    ConfigManager  &m_configManager;
    TextureManager &m_textureManager;
    EntityManager  &m_entityManager;
    SDL_Renderer   *m_renderer;

  public:
    MainSceneSpawner(std::mt19937   &randomGenerator,
                     ConfigManager  &configManager,
                     TextureManager &textureManager,
                     EntityManager  &entityManager,
                     SDL_Renderer   *renderer);

    std::shared_ptr<Entity> spawnPlayer();

    void spawnEnemy(std::shared_ptr<Entity> const &player);
    void spawnSpeedBoostEntity(std::shared_ptr<Entity> const &player);
    void spawnSlownessEntity(std::shared_ptr<Entity> const &player);
    void spawnWalls();
    void spawnBullets(std::shared_ptr<Entity> const &player,
                      Vec2 const                    &mousePosition);
    void spawnItem(std::shared_ptr<Entity> const &player);
};