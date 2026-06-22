#pragma once
#include <Configuration/ConfigAdapter.hpp>
#include <Configuration/ConfigStore.hpp>
#include <Helpers/Vec2.hpp>
#include <SDL.h>
#include <string>


#include "DemoConfigTypes.hpp"

// Demo-specific adapter that sources window/fonts from the engine adapter
// and gameplay configs from the demo adapter store.
class DemoConfigAdapter {
    YerbEngine::ConfigAdapter &m_engine;
    YerbEngine::ConfigStore   &m_demoStore;

    // Thin helper that composes engine parsing helpers for a demo ShapeConfig
    static inline ShapeConfig shapeOr(ShapeConfig              def,
                                      YerbEngine::ConfigStore &store,
                                      std::string const       &base) {
        float w = YerbEngine::ConfigAdapter::floatOr(def.width, store,
                                                     base + ".width");
        float h = YerbEngine::ConfigAdapter::floatOr(def.height, store,
                                                     base + ".height");
        SDL_Color c = YerbEngine::ConfigAdapter::colorOr(def.color, store,
                                                         base + ".color");
        return ShapeConfig{h, w, c};
    }

  public:
    DemoConfigAdapter(YerbEngine::ConfigAdapter &engine,
                      YerbEngine::ConfigStore   &demoStore)
        : m_engine(engine),
          m_demoStore(demoStore) {}

    // Compose engine + demo values
    YerbEngine::GameConfig getGameConfig() {
        auto cfg          = m_engine.getGameConfig();
        cfg.spawnInterval = YerbEngine::ConfigAdapter::u64Or(
            500, m_demoStore, "gameConfig.spawnInterval");
        return cfg;
    }

    // Gameplay configs are read from the demo store
    PlayerConfig getPlayerConfig() {
        PlayerConfig cfg{};
        cfg.baseSpeed = YerbEngine::ConfigAdapter::floatOr(
            6.f, m_demoStore, "playerConfig.baseSpeed");
        cfg.speedBoostMultiplier =
            YerbEngine::ConfigAdapter::floatOr(
                2.f, m_demoStore, "playerConfig.speedBoostMultiplier");
        cfg.slownessMultiplier = YerbEngine::ConfigAdapter::floatOr(
            0.5f, m_demoStore, "playerConfig.slownessMultiplier");
        cfg.shape =
            shapeOr(ShapeConfig{50, 50, SDL_Color{0, 0, 0, 255}},
                    m_demoStore, "playerConfig.shape");
        return cfg;
    }
    ItemConfig getItemConfig() {
        ItemConfig cfg{};
        cfg.spawnPercentage = static_cast<Uint8>(YerbEngine::ConfigAdapter::intOr(
            20, m_demoStore, "itemConfig.spawnPercentage"));
        cfg.lifespan = YerbEngine::ConfigAdapter::u64Or(
            10000, m_demoStore, "itemConfig.lifespan");
        cfg.speed = YerbEngine::ConfigAdapter::floatOr(
            2.f, m_demoStore, "itemConfig.speed");
        cfg.shape    =
            shapeOr(ShapeConfig{40, 40, SDL_Color{0, 0, 0, 255}},
                    m_demoStore, "itemConfig.shape");
        return cfg;
    }
    EnemyConfig getEnemyConfig() {
        EnemyConfig cfg{};
        cfg.spawnPercentage = static_cast<Uint8>(YerbEngine::ConfigAdapter::intOr(
            60, m_demoStore, "enemyConfig.spawnPercentage"));
        cfg.lifespan = YerbEngine::ConfigAdapter::u64Or(
            30000, m_demoStore, "enemyConfig.lifespan");
        cfg.speed = YerbEngine::ConfigAdapter::floatOr(
            3.5f, m_demoStore, "enemyConfig.speed");
        cfg.shape    =
            shapeOr(ShapeConfig{30, 30, SDL_Color{0, 0, 0, 255}},
                    m_demoStore, "enemyConfig.shape");
        return cfg;
    }
    SpeedEffectConfig getSpeedEffectConfig() {
        SpeedEffectConfig cfg{};
        cfg.spawnPercentage = static_cast<Uint8>(YerbEngine::ConfigAdapter::intOr(
            15, m_demoStore, "speedEffectConfig.spawnPercentage"));
        cfg.lifespan = YerbEngine::ConfigAdapter::u64Or(
            9000, m_demoStore, "speedEffectConfig.lifespan");
        cfg.speed = YerbEngine::ConfigAdapter::floatOr(
            2.f, m_demoStore, "speedEffectConfig.speed");
        cfg.shape    =
            shapeOr(ShapeConfig{25, 25, SDL_Color{0, 0, 0, 255}},
                    m_demoStore, "speedEffectConfig.shape");
        return cfg;
    }
    SlownessEffectConfig getSlownessEffectConfig() {
        SlownessEffectConfig cfg{};
        cfg.spawnPercentage = static_cast<Uint8>(YerbEngine::ConfigAdapter::intOr(
            30, m_demoStore, "slownessEffectConfig.spawnPercentage"));
        cfg.lifespan = YerbEngine::ConfigAdapter::u64Or(
            10000, m_demoStore, "slownessEffectConfig.lifespan");
        cfg.speed = YerbEngine::ConfigAdapter::floatOr(
            2.f, m_demoStore, "slownessEffectConfig.speed");
        cfg.shape    =
            shapeOr(ShapeConfig{25, 25, SDL_Color{0, 0, 0, 255}},
                    m_demoStore, "slownessEffectConfig.shape");
        return cfg;
    }
    BulletConfig getBulletConfig() {
        BulletConfig cfg{};
        cfg.lifespan = YerbEngine::ConfigAdapter::u64Or(
            6000, m_demoStore, "bulletConfig.lifespan");
        cfg.speed = YerbEngine::ConfigAdapter::floatOr(
            10.f, m_demoStore, "bulletConfig.speed");
        cfg.shape    = shapeOr(
            ShapeConfig{15, 15, SDL_Color{255, 255, 255, 255}}, m_demoStore,
            "bulletConfig.shape");
        return cfg;
    }
};
