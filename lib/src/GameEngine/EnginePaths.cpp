#include <SDL3/SDL.h>

#include <GameEngine/EnginePaths.hpp>
#include <stdexcept>
#include <string>

namespace YerbEngine {

namespace EnginePaths {

Path resolveDirectory(Path const& preferred, Path const& fallback,
                      char const* const label) {
   std::string const preferredStr = preferred.string();
   if (!preferredStr.empty() && std::filesystem::exists(preferred)) {
      return preferred;
   }

   std::string const fallbackStr = fallback.string();
   bool const hasFallback = preferred != fallback && !fallbackStr.empty() &&
                            std::filesystem::exists(fallback);

   if (hasFallback) {
      SDL_LogWarn(SDL_LOG_CATEGORY_SYSTEM,
                  "%s directory not found at '%s'. Falling back to '%s'.",
                  label, preferredStr.c_str(), fallbackStr.c_str());
      return fallback;
   }

   std::string const attempted = preferred == fallback
                                     ? preferredStr
                                     : preferredStr + "' and '" + fallbackStr;
   SDL_LogError(SDL_LOG_CATEGORY_SYSTEM,
                "%s directory not found (checked '%s').", label,
                attempted.c_str());
   throw std::runtime_error(std::string(label) +
                            " directory not found: " + attempted);
}

}  // namespace EnginePaths

}  // namespace YerbEngine
