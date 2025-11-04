// Deprecated config types have been replaced by an engine/demo split.
#pragma once
#include <Helpers/Vec2.hpp>
#include <SDL.h>
#include <filesystem>
#include <string>

namespace YerbEngine {
  // Legacy-style aggregate used by adapter and various systems. Retained as a
  // stable POD type for convenience. Engine.json provides most fields via the
  // new "engine" namespace; spawnInterval remains a gameplay/demo concern and
  // may fall back to defaults if not present in the active store.
  struct GameConfig {
    Vec2                  windowSize{1600, 900};
    std::string           windowTitle{"YerbEngine"};
    std::filesystem::path fontPath{"./assets/fonts/MotionControl-Bold.otf"};
    Uint64                spawnInterval{500};
    int                   fontSizeSm{38};
    int                   fontSizeMd{48};
    int                   fontSizeLg{68};
  };

    // Engine-wide UI/runtime config
    struct EngineConfig {
        Vec2                  windowSize{1600, 900};
        std::string           windowTitle{"YerbEngine"};
        std::filesystem::path fontPath{"./assets/fonts/MotionControl-Bold.otf"};
        int                   fontSizeSm{38};
        int                   fontSizeMd{48};
        int                   fontSizeLg{68};
    };

    // Demo/gameplay configs remain defined elsewhere (kept by adapter in a named set)
    class ShapeConfig {
      public:
        float     height = 0;
        float     width  = 0;
        SDL_Color color  = {.r = 0, .g = 0, .b = 0, .a = 0};
        ShapeConfig() = default;
        ShapeConfig(float const     height,
                    float const     width,
                    SDL_Color const color)
            : height(height), width(width), color(color) {}
    };

    struct PlayerConfig { float baseSpeed{0}; float speedBoostMultiplier{0}; float slownessMultiplier{0}; ShapeConfig shape; };
    struct ItemConfig   { Uint8 spawnPercentage{0}; Uint64 lifespan{0}; float speed{0}; ShapeConfig shape; };
    struct EnemyConfig  { Uint8 spawnPercentage{0}; Uint64 lifespan{0}; float speed{0}; ShapeConfig shape; };
    struct SpeedEffectConfig { Uint8 spawnPercentage{0}; Uint64 lifespan{0}; float speed{0}; ShapeConfig shape; };
    struct SlownessEffectConfig { Uint8 spawnPercentage{0}; Uint64 lifespan{0}; float speed{0}; ShapeConfig shape; };
    struct BulletConfig { Uint64 lifespan{0}; float speed{0}; ShapeConfig shape; };
}