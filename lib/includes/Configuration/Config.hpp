#pragma once

#include <Helpers/Vec2.hpp>
#include <SDL.h>
#include <filesystem>
namespace YerbEngine {
    class ShapeConfig {
      public:
        float     height = 0;
        float     width  = 0;
        SDL_Color color  = {.r = 0, .g = 0, .b = 0, .a = 0};

        ShapeConfig() = default;
        ShapeConfig(float const     height,
                    float const     width,
                    SDL_Color const color)
            : height(height),
              width(width),
              color(color) {}
    };

    struct GameConfig {
        Vec2                  windowSize;
        std::string           windowTitle;
        std::filesystem::path fontPath;
        Uint64                spawnInterval = 0;
        int                   fontSizeSm;
        int                   fontSizeMd;
        int                   fontSizeLg;
    };

    struct PlayerConfig {
        float       baseSpeed            = 0;
        float       speedBoostMultiplier = 0;
        float       slownessMultiplier   = 0;
        ShapeConfig shape;
    };

    struct ItemConfig {
        Uint8       spawnPercentage = 0;
        Uint64      lifespan        = 0;
        float       speed           = 0;
        ShapeConfig shape;
    };

    struct EnemyConfig {
        Uint8       spawnPercentage = 0;
        Uint64      lifespan        = 0;
        float       speed           = 0;
        ShapeConfig shape;
    };

    struct SpeedEffectConfig {
        Uint8       spawnPercentage = 0;
        Uint64      lifespan        = 0;
        float       speed           = 0;
        ShapeConfig shape;
    };

    struct SlownessEffectConfig {
        Uint8       spawnPercentage = 0;
        Uint64      lifespan        = 0;
        float       speed           = 0;
        ShapeConfig shape;
    };

    struct BulletConfig {
        Uint64      lifespan = 0;
        float       speed    = 0;
        ShapeConfig shape;
    };
} // namespace YerbEngine