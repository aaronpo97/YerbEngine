// Strongly-typed config types used by the engine and adapters.
#pragma once
#include <SDL3/SDL.h>

#include <Helpers/Vec2.hpp>
#include <filesystem>
#include <string>

namespace YerbEngine {

struct GameConfig {
   Vec2 windowSize{1600, 900};
   std::string windowTitle{"YerbEngine"};
   std::filesystem::path fontPath{"./assets/fonts/MotionControl-Bold.otf"};
   Uint64 spawnInterval{500};
   int fontSizeSm{38};
   int fontSizeMd{48};
   int fontSizeLg{68};
};

// Engine-wide UI/runtime config (kept for clarity)
struct EngineConfig {
   Vec2 windowSize{1600, 900};
   std::string windowTitle{"YerbEngine"};
   std::filesystem::path fontPath{"./assets/fonts/MotionControl-Bold.otf"};
   int fontSizeSm{38};
   int fontSizeMd{48};
   int fontSizeLg{68};
};
}  // namespace YerbEngine
