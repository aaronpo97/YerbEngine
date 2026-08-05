#include <Configuration/ConfigStore.hpp>
#include <MenuScene/MenuScene.hpp>
#include <SDL_main.h>
#include <YerbEngine.hpp>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <filesystem>

int main(int    argc,
         char **argv) {
#ifndef __EMSCRIPTEN__
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
#endif

    // Assets/config directories are copied next to the executable by CMake.
    std::filesystem::path const assetsDir = "assets";
    std::filesystem::path const configDir = "config";

    auto const engine = std::make_unique<GameEngine>(assetsDir, configDir);

    std::filesystem::path const demoConfigPath = configDir / "config.json";
    if (std::filesystem::exists(demoConfigPath)) {
        auto demoConfig = YerbEngine::ConfigStore::fromJsonFile(demoConfigPath);
        engine->AddConfig("demo", std::move(demoConfig));
    } else {
        SDL_LogWarn(
            SDL_LOG_CATEGORY_SYSTEM,
            "Demo config file missing at %s; using engine defaults only",
            demoConfigPath.string().c_str());
    }

    // Register demo scenes
    std::shared_ptr<Scene> const menuScene =
        std::make_shared<MenuScene>(engine.get());
    engine->LoadScene("Menu", menuScene);

    engine->run();

    return 0;
}
