#pragma once
#include <Configuration/ConfigStore.hpp>
#include <Configuration/ConfigTypes.hpp>

#include <Helpers/Vec2.hpp>
#include <SDL.h>
#include <string>

namespace YerbEngine {
    // Thin adapter that reconstructs the typed configs from the key-value
    // ConfigStore.
    class ConfigAdapter {

        ConfigStore &m_store;

      public:
        static inline std::string k(std::string const &root,
                                    std::string const &child) {
            return root + "." + child;
        }

        static inline SDL_Color colorOr(SDL_Color          def,
                                        ConfigStore       &store,
                                        std::string const &base) {
            if (store.has(base)) {
                auto v = store.get(base);
                if (std::holds_alternative<SDL_Color>(v)) {
                    return std::get<SDL_Color>(v);
                }
            }
            return def;
        }

        static inline float floatOr(float              def,
                                    ConfigStore       &store,
                                    std::string const &key) {
            auto v = store.get(key);
            if (std::holds_alternative<float>(v))
                return std::get<float>(v);
            if (std::holds_alternative<int>(v))
                return static_cast<float>(std::get<int>(v));
            return def;
        }

        static inline int intOr(int                def,
                                ConfigStore       &store,
                                std::string const &key) {
            auto v = store.get(key);
            if (std::holds_alternative<int>(v))
                return std::get<int>(v);
            return def;
        }

        static inline Uint64 u64Or(Uint64             def,
                                   ConfigStore       &store,
                                   std::string const &key) {
            auto v = store.get(key);
            if (std::holds_alternative<int>(v))
                return static_cast<Uint64>(std::get<int>(v));
            return def;
        }

        static inline std::string strOr(std::string        def,
                                        ConfigStore       &store,
                                        std::string const &key) {
            auto v = store.get(key);
            if (std::holds_alternative<std::string>(v))
                return std::get<std::string>(v);
            return def;
        }

        static inline Vec2 vec2Or(Vec2               def,
                                  ConfigStore       &store,
                                  std::string const &base) {
            float w = floatOr(def.x(), store, base + ".width");
            float h = floatOr(def.y(), store, base + ".height");
            return Vec2{w, h};
        }

        explicit ConfigAdapter(ConfigStore &store) : m_store(store) {}

        GameConfig getGameConfig() {
            GameConfig cfg;
            // Primary source: engine.json under engine.*
            cfg.windowTitle =
                strOr("YerbEngine", m_store, "engine.window.title");
            cfg.windowSize =
                vec2Or(Vec2{1600, 900}, m_store, "engine.window.size");
            cfg.fontPath   = strOr("./assets/fonts/MotionControl-Bold.otf",
                                   m_store, "engine.fonts.path");
            cfg.fontSizeSm = intOr(38, m_store, "engine.fonts.sizes.sm");
            cfg.fontSizeMd = intOr(48, m_store, "engine.fonts.sizes.md");
            cfg.fontSizeLg = intOr(68, m_store, "engine.fonts.sizes.lg");

            // Gameplay-driven, stays under demo config
            cfg.spawnInterval = u64Or(500, m_store, "gameConfig.spawnInterval");

            return cfg;
        }
    };
} // namespace YerbEngine
