#pragma once
#include <Configuration/ConfigAdapter.hpp>
#include <Configuration/ConfigStore.hpp>
#include <Helpers/Vec2.hpp>
#include <SDL.h>
#include <string>

#include <Configuration/ConfigDeprecated.hpp>
#include <Configuration/ConfigStore.hpp>

#include "DemoConfigTypes.hpp"

// Demo-specific adapter that sources window/fonts from the engine adapter
// and gameplay configs from the demo adapter store.
class DemoConfigAdapter {
    YerbEngine::ConfigAdapter &m_engine;
    YerbEngine::ConfigStore   &m_demoStore;

    // Local helpers to read values from the demo store
    static inline SDL_Color colorOr(SDL_Color                    def,
                                    YerbEngine::ConfigStore     &store,
                                    std::string const           &base) {
        if (store.has(base)) {
            auto v = store.get(base);
            if (std::holds_alternative<SDL_Color>(v)) {
                return std::get<SDL_Color>(v);
            }
        }
        return def;
    }
    static inline float floatOr(float                        def,
                                YerbEngine::ConfigStore     &store,
                                std::string const           &key) {
        auto v = store.get(key);
        if (std::holds_alternative<float>(v))
            return std::get<float>(v);
        if (std::holds_alternative<int>(v))
            return static_cast<float>(std::get<int>(v));
        return def;
    }
    static inline int intOr(int                          def,
                            YerbEngine::ConfigStore     &store,
                            std::string const           &key) {
        auto v = store.get(key);
        if (std::holds_alternative<int>(v))
            return std::get<int>(v);
        return def;
    }
    static inline Uint64 u64Or(Uint64                      def,
                               YerbEngine::ConfigStore     &store,
                               std::string const           &key) {
        auto v = store.get(key);
        if (std::holds_alternative<int>(v))
            return static_cast<Uint64>(std::get<int>(v));
        return def;
    }
    static inline ShapeConfig shapeOr(ShapeConfig                  def,
                                      YerbEngine::ConfigStore     &store,
                                      std::string const           &base) {
        float     w = floatOr(def.width, store, base + ".width");
        float     h = floatOr(def.height, store, base + ".height");
        SDL_Color c = colorOr(def.color, store, base + ".color");
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
        cfg.spawnInterval = u64Or(500, m_demoStore, "gameConfig.spawnInterval");
        return cfg;
    }

    // Gameplay configs are read from the demo store
    PlayerConfig getPlayerConfig() {
        PlayerConfig cfg{};
        cfg.baseSpeed =
            floatOr(6.f, m_demoStore, "playerConfig.baseSpeed");
        cfg.speedBoostMultiplier =
            floatOr(2.f, m_demoStore, "playerConfig.speedBoostMultiplier");
        cfg.slownessMultiplier = floatOr(
            0.5f, m_demoStore, "playerConfig.slownessMultiplier");
        cfg.shape =
            shapeOr(ShapeConfig{50, 50, SDL_Color{0, 0, 0, 255}},
                    m_demoStore, "playerConfig.shape");
        return cfg;
    }
    ItemConfig getItemConfig() {
        ItemConfig cfg{};
        cfg.spawnPercentage = static_cast<Uint8>(
            intOr(20, m_demoStore, "itemConfig.spawnPercentage"));
        cfg.lifespan = u64Or(10000, m_demoStore, "itemConfig.lifespan");
        cfg.speed    = floatOr(2.f, m_demoStore, "itemConfig.speed");
        cfg.shape    =
            shapeOr(ShapeConfig{40, 40, SDL_Color{0, 0, 0, 255}},
                    m_demoStore, "itemConfig.shape");
        return cfg;
    }
    EnemyConfig getEnemyConfig() {
        EnemyConfig cfg{};
        cfg.spawnPercentage = static_cast<Uint8>(intOr(
            60, m_demoStore, "enemyConfig.spawnPercentage"));
        cfg.lifespan = u64Or(30000, m_demoStore, "enemyConfig.lifespan");
        cfg.speed    = floatOr(3.5f, m_demoStore, "enemyConfig.speed");
        cfg.shape    =
            shapeOr(ShapeConfig{30, 30, SDL_Color{0, 0, 0, 255}},
                    m_demoStore, "enemyConfig.shape");
        return cfg;
    }
    SpeedEffectConfig getSpeedEffectConfig() {
        SpeedEffectConfig cfg{};
        cfg.spawnPercentage = static_cast<Uint8>(intOr(
            15, m_demoStore, "speedEffectConfig.spawnPercentage"));
        cfg.lifespan = u64Or(9000, m_demoStore, "speedEffectConfig.lifespan");
        cfg.speed    = floatOr(2.f, m_demoStore, "speedEffectConfig.speed");
        cfg.shape    =
            shapeOr(ShapeConfig{25, 25, SDL_Color{0, 0, 0, 255}},
                    m_demoStore, "speedEffectConfig.shape");
        return cfg;
    }
    SlownessEffectConfig getSlownessEffectConfig() {
        SlownessEffectConfig cfg{};
        cfg.spawnPercentage = static_cast<Uint8>(intOr(
            30, m_demoStore, "slownessEffectConfig.spawnPercentage"));
        cfg.lifespan = u64Or(10000, m_demoStore, "slownessEffectConfig.lifespan");
        cfg.speed    = floatOr(2.f, m_demoStore, "slownessEffectConfig.speed");
        cfg.shape    =
            shapeOr(ShapeConfig{25, 25, SDL_Color{0, 0, 0, 255}},
                    m_demoStore, "slownessEffectConfig.shape");
        return cfg;
    }
    BulletConfig getBulletConfig() {
        BulletConfig cfg{};
        cfg.lifespan = u64Or(6000, m_demoStore, "bulletConfig.lifespan");
        cfg.speed    = floatOr(10.f, m_demoStore, "bulletConfig.speed");
        cfg.shape    = shapeOr(
            ShapeConfig{15, 15, SDL_Color{255, 255, 255, 255}}, m_demoStore,
            "bulletConfig.shape");
        return cfg;
    }
};
