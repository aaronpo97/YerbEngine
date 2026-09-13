#pragma once
#include <SDL3/SDL.h>

#include <Configuration/ConfigAdapter.hpp>
#include <Configuration/ConfigReader.hpp>
#include <Configuration/ConfigStore.hpp>
#include <Helpers/Vec2.hpp>
#include <string>

#include "DemoConfigTypes.hpp"

// Demo-specific adapter that sources window/fonts from the engine adapter
// and gameplay configs from the demo config store.
class DemoConfigAdapter {
   YerbEngine::ConfigAdapter& m_engine;
   YerbEngine::ConfigReader m_demo;

   ShapeConfig shapeOr(std::string const& base,
                       ShapeConfig const& fallback) const {
      return ShapeConfig{
          .height = m_demo.floatOr(base + ".height", fallback.height),
          .width = m_demo.floatOr(base + ".width", fallback.width),
          .color = m_demo.colorOr(base + ".color", fallback.color),
      };
   }

   static constexpr ShapeConfig squareOf(float const size) {
      return ShapeConfig{
          .height = size,
          .width = size,
          .color = SDL_Color{0, 0, 0, 255},
      };
   }

  public:
   DemoConfigAdapter(YerbEngine::ConfigAdapter& engine,
                     YerbEngine::ConfigStore& demoStore)
       : m_engine(engine), m_demo(demoStore) {}

   // The window and the fonts belong to the engine.
   YerbEngine::GameConfig getGameConfig() const {
      auto cfg = m_engine.getGameConfig();
      cfg.spawnInterval = m_demo.u64Or("gameConfig.spawnInterval", 500);
      return cfg;
   }

   PlayerConfig getPlayerConfig() const {
      return PlayerConfig{
          .baseSpeed = m_demo.floatOr("playerConfig.baseSpeed", 6.f),
          .speedBoostMultiplier =
              m_demo.floatOr("playerConfig.speedBoostMultiplier", 2.f),
          .slownessMultiplier =
              m_demo.floatOr("playerConfig.slownessMultiplier", 0.5f),
          .shape = shapeOr("playerConfig.shape", squareOf(50)),
      };
   }

   ItemConfig getItemConfig() const {
      return ItemConfig{
          .spawnPercentage = static_cast<Uint8>(
              m_demo.intOr("itemConfig.spawnPercentage", 20)),
          .lifespan = m_demo.u64Or("itemConfig.lifespan", 10000),
          .speed = m_demo.floatOr("itemConfig.speed", 2.f),
          .shape = shapeOr("itemConfig.shape", squareOf(40)),
      };
   }

   EnemyConfig getEnemyConfig() const {
      return EnemyConfig{
          .spawnPercentage = static_cast<Uint8>(
              m_demo.intOr("enemyConfig.spawnPercentage", 60)),
          .lifespan = m_demo.u64Or("enemyConfig.lifespan", 30000),
          .speed = m_demo.floatOr("enemyConfig.speed", 3.5f),
          .shape = shapeOr("enemyConfig.shape", squareOf(30)),
      };
   }

   SpeedEffectConfig getSpeedEffectConfig() const {
      return SpeedEffectConfig{
          .spawnPercentage = static_cast<Uint8>(
              m_demo.intOr("speedEffectConfig.spawnPercentage", 15)),
          .lifespan = m_demo.u64Or("speedEffectConfig.lifespan", 9000),
          .speed = m_demo.floatOr("speedEffectConfig.speed", 2.f),
          .shape = shapeOr("speedEffectConfig.shape", squareOf(25)),
      };
   }

   SlownessEffectConfig getSlownessEffectConfig() const {
      return SlownessEffectConfig{
          .spawnPercentage = static_cast<Uint8>(
              m_demo.intOr("slownessEffectConfig.spawnPercentage", 30)),
          .lifespan = m_demo.u64Or("slownessEffectConfig.lifespan", 10000),
          .speed = m_demo.floatOr("slownessEffectConfig.speed", 2.f),
          .shape = shapeOr("slownessEffectConfig.shape", squareOf(25)),
      };
   }

   BulletConfig getBulletConfig() const {
      constexpr ShapeConfig defaultShape{
          .height = 15,
          .width = 15,
          .color = SDL_Color{255, 255, 255, 255},
      };

      return BulletConfig{
          .lifespan = m_demo.u64Or("bulletConfig.lifespan", 6000),
          .speed = m_demo.floatOr("bulletConfig.speed", 10.f),
          .shape = shapeOr("bulletConfig.shape", defaultShape),
      };
   }
};
