#include "includes/GameEngine/GameEngine.hpp"
#include <SDL_main.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

int main(int argc, char **argv) {
#ifndef __EMSCRIPTEN__
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
#endif

    const auto engine = std::make_unique<GameEngine>();

    engine->run();

    return 0;
}