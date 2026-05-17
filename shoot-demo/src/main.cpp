#include "shoot-demo/includes/MenuScene/MenuScene.hpp"
#include <SDL_main.h>
#include <YerbEngine/YerbEngine.hpp>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

int main(int argc, char **argv) {
#ifndef __EMSCRIPTEN__
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
#endif

    const auto engine = std::make_unique<GameEngine>();

    // Register demo scenes
    std::shared_ptr<Scene> const menuScene =
        std::make_shared<MenuScene>(engine.get());
    engine->loadScene("Menu", menuScene);

    engine->run();

    return 0;
}