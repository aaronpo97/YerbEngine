#pragma once
#include <SDL3/SDL.h>

#include <Configuration/ConfigReader.hpp>
#include <Configuration/ConfigStore.hpp>
#include <Configuration/ConfigTypes.hpp>
#include <Helpers/Vec2.hpp>
#include <string>

namespace YerbEngine {
// Thin adapter that reconstructs the typed configs from the key-value
// ConfigStore.
class ConfigAdapter : public ConfigReader {
  public:
   explicit ConfigAdapter(ConfigStore& store) : ConfigReader(store) {}

   GameConfig getGameConfig() const {
      GameConfig cfg;
      // Primary source: engine.json under engine.*
      cfg.windowTitle = strOr("engine.window.title", "YerbEngine");
      cfg.windowSize = vec2Or("engine.window.size", Vec2{1600, 900});
      cfg.fontPath =
          strOr("engine.fonts.path", "./assets/fonts/MotionControl-Bold.otf");
      cfg.fontSizeSm = intOr("engine.fonts.sizes.sm", 38);
      cfg.fontSizeMd = intOr("engine.fonts.sizes.md", 48);
      cfg.fontSizeLg = intOr("engine.fonts.sizes.lg", 68);

      return cfg;
   }
};
}  // namespace YerbEngine
