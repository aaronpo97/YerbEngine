#pragma once

#include <cstddef>
#include <string>
#include <string_view>

namespace YerbEngine {

using AudioSampleId = std::string;
using AudioTrackId = std::string;

/**
 * The playback end of the sample pipeline.
 *
 * AudioSampleBuffer decides which samples a frame should start and hands
 * them here; the implementation owns the mixer and the voices.
 */
class IAudioSink {
  public:
   /**
    * How many samples one frame may start.
    *
    * The buffer stops at this many per update so a backlog is spread over
    * several frames rather than arriving as a single wall of sound, and so
    * the sink's voice pool is never oversubscribed.
    */
   static constexpr size_t MAX_SAMPLES_PER_FRAME = 4;

   virtual ~IAudioSink() = default;

   /**
    * Starts a sample.
    *
    * @param sample The id of the sample to play.
    * @param loops The number of times to loop the sample (0 for no loop).
    */
   virtual void playSample(std::string_view sample, int loops = 0) = 0;
};

}  // namespace YerbEngine
