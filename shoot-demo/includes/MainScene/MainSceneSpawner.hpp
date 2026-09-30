#pragma once
#include <Configuration/DemoConfigAdapter.hpp>
#include <YerbEngine.hpp>
#include <random>

class MainSceneSpawner {
   using Entity = YerbEngine::Entity;
   using Vec2 = YerbEngine::Vec2;

   std::mt19937& m_randomGenerator;
   DemoConfigAdapter& m_config;
   YerbEngine::VideoManager& m_videoManager;
   YerbEngine::TextureManager& m_textureManager;
   YerbEngine::EntityManager& m_entityManager;

  public:
   MainSceneSpawner(std::mt19937& randomGenerator, DemoConfigAdapter& config,
                    YerbEngine::TextureManager& textureManager,
                    YerbEngine::EntityManager& entityManager,
                    YerbEngine::VideoManager& videoManager);

   std::shared_ptr<Entity> spawnPlayer();

   void spawnEnemy(std::shared_ptr<Entity> const& player);
   void spawnSpeedBoostEntity(std::shared_ptr<Entity> const& player);
   void spawnSlownessEntity(std::shared_ptr<Entity> const& player);
   void spawnWalls();
   void spawnBullets(std::shared_ptr<Entity> const& player,
                     Vec2 const& mousePosition);
   void spawnItem(std::shared_ptr<Entity> const& player);
};
