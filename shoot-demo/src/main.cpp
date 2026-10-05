#include <SDL3/SDL_main.h>

#include <DemoServices.hpp>
#include <GameEngine/EnginePaths.hpp>
#include <MenuScene/MenuScene.hpp>
#include <SystemManagement/IClock.hpp>
#include <SystemManagement/SdlRuntime.hpp>
#include <YerbEngine.hpp>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <filesystem>
#include <memory>
#include <optional>

using namespace YerbEngine;

int main(int argc, char** argv) {
#ifndef __EMSCRIPTEN__
   SDL_SetLogPriorities(SDL_LOG_PRIORITY_VERBOSE);
#endif

   /*
    * Declared first so that it is destroyed last: SDL is shut down only
    * once the managers that use its subsystems are gone.
    */
   SdlRuntime const sdl;

   Path const assetsDir =
       EnginePaths::resolveDirectory("assets", "assets", "Assets");
   Path const configDir =
       EnginePaths::resolveDirectory("config", "config", "Config");

   AudioManifest const audio =
       loadAudioManifest(configDir / "audio.json", assetsDir);

   /*
    * The engine's own configuration is read before the services exist,
    * because the font sizes it holds are themselves dependencies.
    */
   std::unique_ptr<ConfigStore> const engineStore =
       ConfigStore::fromJsonFile(configDir / "engine.json");
   ConfigAdapter engineConfig{*engineStore};
   GameConfig const gameConfig = engineConfig.getGameConfig();

   Path const demoConfigPath = configDir / "config.json";
   if (!std::filesystem::exists(demoConfigPath)) {
      SDL_LogWarn(SDL_LOG_CATEGORY_SYSTEM,
                  "Demo config file missing at %s; using defaults",
                  demoConfigPath.string().c_str());
   }

   std::unique_ptr<ConfigStore> const demoStore =
       std::filesystem::exists(demoConfigPath)
           ? ConfigStore::fromJsonFile(demoConfigPath)
           : std::make_unique<ConfigStore>(
                 std::make_unique<EmptyConfigProvider>());

   /*
    * Declared ahead of the engine so that they outlive it: the engine owns
    * the scenes, and every scene holds a reference to this bundle. Both are
    * filled in below, once the engine they name as their router exists.
    */
   std::optional<DemoConfigAdapter> demoConfig;
   std::optional<DemoServices> services;

   /*
    * The collaborators a scene is lent, in dependency order. Destruction
    * runs in reverse, which is the order SDL asks for: the engine lets go of
    * its scenes, textures go before the renderer that owns them, and every
    * subsystem is torn down while SDL is still up, because `sdl` above
    * reaches SDL_Quit() only once all of these are gone.
    */
   SdlClock const clock{};
   AudioManager mixer{audio.options};
   AudioSampleBuffer sampleBuffer{mixer, clock};
   VideoManager video{engineConfig};
   FontManager fonts{gameConfig.fontPath,
                     FontSizes{.sm = gameConfig.fontSizeSm,
                               .md = gameConfig.fontSizeMd,
                               .lg = gameConfig.fontSizeLg}};
   // The renderer belongs to the window, so it is handed out by it.
   TextureManager textures{RendererHandle{video.getRenderer()}};
   GameEngine engine{video, clock};

   demoConfig.emplace(engineConfig, *demoStore);
   services.emplace(DemoServices{{.video = video,
                                  .fonts = fonts,
                                  .textures = textures,
                                  .audio = sampleBuffer,
                                  .mixer = mixer,
                                  .clock = clock,
                                  .router = engine},
                                 *demoConfig});

   sampleBuffer.setCooldowns(audio.sampleCooldowns);

   engine.LoadScene("Menu", std::make_shared<MenuScene>(*services));

   engine.run();

   return 0;
}
