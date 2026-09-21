#pragma once

#include <SDL3/SDL.h>

namespace YerbEngine {

/**
 * The engine's source of elapsed time.
 *
 * Anything that decides *when* something happens reads the time from here
 * rather than from SDL directly, so a test can place events at exact
 * millisecond offsets instead of sleeping for them.
 */
class IClock {
  public:
   virtual ~IClock() = default;

   /** Milliseconds elapsed on a monotonic timeline. */
   virtual Uint64 nowMs() const = 0;
};

/** The real clock: milliseconds since SDL was initialized. */
class SdlClock final : public IClock {
  public:
   Uint64 nowMs() const override { return SDL_GetTicks(); }
};

}  // namespace YerbEngine
