#include <SDL3/SDL_main.h>

#include <Configuration/ConfigStore.hpp>
#include <DemoServices.hpp>
#include <MenuScene/MenuScene.hpp>
#include <YerbEngine.hpp>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <filesystem>

using namespace YerbEngine;

int main(int argc, char** argv) {
#ifndef __EMSCRIPTEN__
   SDL_SetLogPriorities(SDL_LOG_PRIORITY_VERBOSE);
#endif

   // Assets/config directories are copied next to the executable by CMake.
   std::filesystem::path const assetsDir = "assets";
   std::filesystem::path const configDir = "config";

   AudioManifest const audio =
       loadAudioManifest(configDir / "audio.json", assetsDir);

   auto const engine =
       std::make_unique<GameEngine>(assetsDir, configDir, audio.options);

   std::filesystem::path const demoConfigPath = configDir / "config.json";
   if (std::filesystem::exists(demoConfigPath)) {
      auto demoConfig = ConfigStore::fromJsonFile(demoConfigPath);
      engine->AddConfig("demo", std::move(demoConfig));
   } else {
      SDL_LogWarn(SDL_LOG_CATEGORY_SYSTEM,
                  "Demo config file missing at %s; using engine defaults only",
                  demoConfigPath.string().c_str());
   }

   DemoConfigAdapter demoConfig{engine->GetConfig(),
                                engine->GetConfigStore("demo")};

   SdlClock const clock;

   /*
    * Outlives the scenes by construction: the engine owns the scenes, and
    * every scene holds a reference to this bundle.
    */
   DemoServices const services{{.video = engine->getVideoManager(),
                                .fonts = engine->getFontManager(),
                                .textures = engine->getTextureManager(),
                                .audio = engine->getAudioSampleBuffer(),
                                .mixer = engine->getAudioManager(),
                                .clock = clock,
                                .router = *engine},
                               demoConfig};

   engine->getAudioSampleBuffer().setCooldowns(audio.sampleCooldowns);

   engine->LoadScene("Menu", std::make_shared<MenuScene>(services));

   engine->run();

   return 0;
}
