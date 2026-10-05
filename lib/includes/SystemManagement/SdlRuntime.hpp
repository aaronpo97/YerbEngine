#pragma once

namespace YerbEngine {

/**
 * Owns the SDL library's lifetime.
 *
 * Subsystems are started by whoever needs them (the video and audio
 * managers); this shuts the library down once they are all gone. Declare one
 * ahead of everything else in the composition root so it is destroyed last.
 */
class SdlRuntime {
  public:
   SdlRuntime() = default;
   ~SdlRuntime();

   SdlRuntime(SdlRuntime const&) = delete;
   SdlRuntime& operator=(SdlRuntime const&) = delete;
};

}  // namespace YerbEngine
