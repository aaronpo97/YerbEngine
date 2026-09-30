#include <SDL3/SDL_main.h>

#include <Configuration/ConfigStore.hpp>
#include <MenuScene/MenuScene.hpp>
#include <PongServices.hpp>
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

   std::filesystem::path const assetsDir = "pong-assets";
   std::filesystem::path const configDir = "pong-config";

   AudioManifest const audio =
       loadAudioManifest(configDir / "audio.json", assetsDir);

   auto const engine =
       std::make_unique<GameEngine>(assetsDir, configDir, audio.options);

   std::filesystem::path const pongConfigPath = configDir / "config.json";
   if (std::filesystem::exists(pongConfigPath)) {
      auto pongConfig = ConfigStore::fromJsonFile(pongConfigPath);
      engine->AddConfig("demo", std::move(pongConfig));
   } else {
      SDL_LogWarn(SDL_LOG_CATEGORY_SYSTEM,
                  "Pong config file missing at %s; using engine defaults only",
                  pongConfigPath.string().c_str());
   }

   PongConfigAdapter pongConfig{engine->GetConfig(),
                                engine->GetConfigStore("demo")};

   SdlClock const clock;

   /*
    * Outlives the scenes by construction: the engine owns the scenes, and
    * every scene holds a reference to this bundle.
    */
   PongServices const services{{.video = engine->getVideoManager(),
                                .fonts = engine->getFontManager(),
                                .textures = engine->getTextureManager(),
                                .audio = engine->getAudioSampleBuffer(),
                                .mixer = engine->getAudioManager(),
                                .clock = clock,
                                .router = *engine},
                               pongConfig};

   engine->getAudioSampleBuffer().setCooldowns(audio.sampleCooldowns);

   engine->LoadScene("Menu", std::make_shared<MenuScene>(services));

   engine->run();

   return 0;
}
