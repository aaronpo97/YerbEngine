#pragma once
#include "ConfigDeprecated.hpp"
#include "ConfigStore.hpp"
#include <string>

namespace YerbEngine {
    // Thin adapter that reconstructs the typed configs from the key-value ConfigStore.
    class ConfigAdapter {
        ConfigStore &m_store;

        static inline std::string k(std::string const &root, std::string const &child) {
            return root + "." + child;
        }

        static inline SDL_Color colorOr(SDL_Color def, ConfigStore &store, std::string const &base) {
            if (store.has(base)) {
                auto v = store.get(base);
                if (std::holds_alternative<SDL_Color>(v)) {
                    return std::get<SDL_Color>(v);
                }
            }
            return def;
        }

        static inline float floatOr(float def, ConfigStore &store, std::string const &key) {
            auto v = store.get(key);
            if (std::holds_alternative<float>(v)) return std::get<float>(v);
            if (std::holds_alternative<int>(v)) return static_cast<float>(std::get<int>(v));
            return def;
        }

        static inline int intOr(int def, ConfigStore &store, std::string const &key) {
            auto v = store.get(key);
            if (std::holds_alternative<int>(v)) return std::get<int>(v);
            return def;
        }

        static inline Uint64 u64Or(Uint64 def, ConfigStore &store, std::string const &key) {
            auto v = store.get(key);
            if (std::holds_alternative<int>(v)) return static_cast<Uint64>(std::get<int>(v));
            return def;
        }

        static inline std::string strOr(std::string def, ConfigStore &store, std::string const &key) {
            auto v = store.get(key);
            if (std::holds_alternative<std::string>(v)) return std::get<std::string>(v);
            return def;
        }

        static inline Vec2 vec2Or(Vec2 def, ConfigStore &store, std::string const &base) {
            float w = floatOr(def.x(), store, base + ".width");
            float h = floatOr(def.y(), store, base + ".height");
            return Vec2{w, h};
        }

        static inline ShapeConfig shapeOr(ShapeConfig def, ConfigStore &store, std::string const &base) {
            float w = floatOr(def.width, store, base + ".width");
            float h = floatOr(def.height, store, base + ".height");
            SDL_Color c = colorOr(def.color, store, base + ".color");
            return ShapeConfig{h, w, c};
        }

    public:
        explicit ConfigAdapter(ConfigStore &store) : m_store(store) {}

        GameConfig getGameConfig() {
            GameConfig cfg;
            // Primary source: engine.json under engine.*
            cfg.windowTitle = strOr("YerbEngine", m_store, "engine.window.title");
            cfg.windowSize = vec2Or(Vec2{1600, 900}, m_store, "engine.window.size");
            cfg.fontPath = strOr("./assets/fonts/MotionControl-Bold.otf", m_store, "engine.fonts.path");
            cfg.fontSizeSm = intOr(38, m_store, "engine.fonts.sizes.sm");
            cfg.fontSizeMd = intOr(48, m_store, "engine.fonts.sizes.md");
            cfg.fontSizeLg = intOr(68, m_store, "engine.fonts.sizes.lg");

            // Fallbacks for legacy demo config names if engine.* not provided
            if (cfg.windowTitle.empty()) {
                cfg.windowTitle = strOr("Project Yerb", m_store, "gameConfig.windowTitle");
            }
            if (cfg.fontPath.empty()) {
                cfg.fontPath = strOr("./assets/fonts/MotionControl-Bold.otf", m_store, "gameConfig.fontPath");
            }
            // If engine.window.size wasn't found, the vec2Or default was used; we also try legacy path
            if (!m_store.has("engine.window.size.width") &&
                m_store.has("gameConfig.windowSize.width")) {
                cfg.windowSize = vec2Or(cfg.windowSize, m_store, "gameConfig.windowSize");
            }

            // spawnInterval stays sourced from demo config when available
            cfg.spawnInterval = u64Or(500, m_store, "gameConfig.spawnInterval");
            return cfg;
        }

        PlayerConfig getPlayerConfig() {
            PlayerConfig cfg{};
            cfg.baseSpeed = floatOr(6.f, m_store, "playerConfig.baseSpeed");
            cfg.speedBoostMultiplier = floatOr(2.f, m_store, "playerConfig.speedBoostMultiplier");
            cfg.slownessMultiplier = floatOr(0.5f, m_store, "playerConfig.slownessMultiplier");
            cfg.shape = shapeOr(ShapeConfig{50, 50, SDL_Color{0,0,0,255}}, m_store, "playerConfig.shape");
            return cfg;
        }

        ItemConfig getItemConfig() {
            ItemConfig cfg{};
            cfg.spawnPercentage = static_cast<Uint8>(intOr(20, m_store, "itemConfig.spawnPercentage"));
            cfg.lifespan = u64Or(10000, m_store, "itemConfig.lifespan");
            cfg.speed = floatOr(2.f, m_store, "itemConfig.speed");
            cfg.shape = shapeOr(ShapeConfig{40, 40, SDL_Color{0,0,0,255}}, m_store, "itemConfig.shape");
            return cfg;
        }

        EnemyConfig getEnemyConfig() {
            EnemyConfig cfg{};
            cfg.spawnPercentage = static_cast<Uint8>(intOr(60, m_store, "enemyConfig.spawnPercentage"));
            cfg.lifespan = u64Or(30000, m_store, "enemyConfig.lifespan");
            cfg.speed = floatOr(3.5f, m_store, "enemyConfig.speed");
            cfg.shape = shapeOr(ShapeConfig{30, 30, SDL_Color{0,0,0,255}}, m_store, "enemyConfig.shape");
            return cfg;
        }

        SpeedEffectConfig getSpeedEffectConfig() {
            SpeedEffectConfig cfg{};
            cfg.spawnPercentage = static_cast<Uint8>(intOr(15, m_store, "speedEffectConfig.spawnPercentage"));
            cfg.lifespan = u64Or(9000, m_store, "speedEffectConfig.lifespan");
            cfg.speed = floatOr(2.f, m_store, "speedEffectConfig.speed");
            cfg.shape = shapeOr(ShapeConfig{25, 25, SDL_Color{0,0,0,255}}, m_store, "speedEffectConfig.shape");
            return cfg;
        }

        SlownessEffectConfig getSlownessEffectConfig() {
            SlownessEffectConfig cfg{};
            cfg.spawnPercentage = static_cast<Uint8>(intOr(30, m_store, "slownessEffectConfig.spawnPercentage"));
            cfg.lifespan = u64Or(10000, m_store, "slownessEffectConfig.lifespan");
            cfg.speed = floatOr(2.f, m_store, "slownessEffectConfig.speed");
            cfg.shape = shapeOr(ShapeConfig{25, 25, SDL_Color{0,0,0,255}}, m_store, "slownessEffectConfig.shape");
            return cfg;
        }

        BulletConfig getBulletConfig() {
            BulletConfig cfg{};
            cfg.lifespan = u64Or(6000, m_store, "bulletConfig.lifespan");
            cfg.speed = floatOr(10.f, m_store, "bulletConfig.speed");
            cfg.shape = shapeOr(ShapeConfig{15, 15, SDL_Color{255,255,255,255}}, m_store, "bulletConfig.shape");
            return cfg;
        }
    };
} // namespace YerbEngine
