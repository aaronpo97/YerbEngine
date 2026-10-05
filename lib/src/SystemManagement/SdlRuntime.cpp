#include <SDL3/SDL.h>

#include <SystemManagement/SdlRuntime.hpp>

namespace YerbEngine {

SdlRuntime::~SdlRuntime() {
   SDL_Quit();
   SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM, "SDL shut down.");
}

}  // namespace YerbEngine
