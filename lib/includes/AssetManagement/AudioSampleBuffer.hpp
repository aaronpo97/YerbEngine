#pragma once

#include <SDL3/SDL.h>

#include <SystemManagement/IAudioSink.hpp>
#include <SystemManagement/IClock.hpp>
#include <array>
#include <string_view>
#include <unordered_map>
namespace YerbEngine {
enum class PriorityLevel : uint8_t {
   BACKGROUND,
   STANDARD,
   IMPORTANT,
   CRITICAL
};
class AudioSampleBuffer {
   struct QueuedSample {
      AudioSampleId sample;
      PriorityLevel priority;
      Uint64 timestamp;

      constexpr bool operator<(QueuedSample const& other) const {
         if (priority != other.priority) {
            return priority < other.priority;
         }
         return timestamp > other.timestamp;
      }
   };

   static constexpr size_t SAMPLE_BUFFER_CAPACITY = 64;
   static constexpr size_t TRIM_LIMIT = 20;
   static constexpr Uint64 STALE_SAMPLE_MS = 500;

   std::array<QueuedSample, SAMPLE_BUFFER_CAPACITY> m_sampleBuffer{};
   size_t m_head = 0;
   size_t m_tail = 0;
   size_t m_size = 0;
   std::unordered_map<AudioSampleId, Uint64> m_lastPlayTimes;
   IAudioSink& m_audioSink;
   IClock const& m_clock;

   static constexpr Uint64 MIN_REPLAY_INTERVAL = 50;
   std::unordered_map<AudioSampleId, Uint64> m_cooldowns;

   [[nodiscard]] bool isBufferFull() const;
   void pushSample(QueuedSample const& queuedSample);
   void removeExpiredSamples(Uint64 currentTime);
   void removeAtOffset(size_t offset);
   [[nodiscard]] bool evictLowerPrioritySample(PriorityLevel incoming);
   void trimLowPrioritySamples();

  public:
   AudioSampleBuffer(IAudioSink& audioSink, IClock const& clock);
   void queueSample(std::string_view sample, PriorityLevel priority);
   void setCooldown(std::string_view sample, Uint64 cooldownMs);
   void setCooldowns(std::unordered_map<AudioSampleId, Uint64> cooldowns);
   void update();
};
}  // namespace YerbEngine
