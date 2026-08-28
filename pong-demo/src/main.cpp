#include <SDL_main.h>

#include <Configuration/AudioConfig.hpp>
#include <Configuration/ConfigStore.hpp>
#include <MenuScene/MenuScene.hpp>
#include <YerbEngine.hpp>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <filesystem>

int main(int argc, char** argv) {
#ifndef __EMSCRIPTEN__
   SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
#endif

   std::filesystem::path const assetsDir = "pong-assets";
   std::filesystem::path const configDir = "pong-config";

   YerbEngine::AudioInitOptions audioOptions =
       PongAudio::buildAudioInitOptions(assetsDir);

   auto const engine =
       std::make_unique<GameEngine>(assetsDir, configDir, audioOptions);

   std::filesystem::path const demoConfigPath = configDir / "config.json";
   if (std::filesystem::exists(demoConfigPath)) {
      auto demoConfig = YerbEngine::ConfigStore::fromJsonFile(demoConfigPath);
      engine->AddConfig("demo", std::move(demoConfig));
   } else {
      SDL_LogWarn(SDL_LOG_CATEGORY_SYSTEM,
                  "Demo config file missing at %s; using engine defaults only",
                  demoConfigPath.string().c_str());
   }

   std::shared_ptr<Scene> const menuScene =
       std::make_shared<MenuScene>(engine.get());
   engine->LoadScene("Menu", menuScene);

   engine->getAudioSampleBuffer().setCooldowns(PongAudio::sampleCooldowns());

   engine->run();

   return 0;
}
