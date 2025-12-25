#pragma once

#include <SystemManagement/AudioManager.hpp>
#include <array>
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
            AudioSample   sample;
            PriorityLevel priority;
            Uint64        timestamp;

            constexpr bool operator<(QueuedSample const &other) const {
                if (priority != other.priority) {
                    return priority < other.priority;
                }
                return timestamp > other.timestamp;
            }
        };

        static constexpr size_t SAMPLE_BUFFER_CAPACITY = 64;
        static constexpr size_t TRIM_LIMIT             = 20;
        static constexpr Uint64 STALE_SAMPLE_MS        = 500;

        std::array<QueuedSample, SAMPLE_BUFFER_CAPACITY> m_sampleBuffer{};
        size_t                                           m_head = 0;
        size_t                                           m_tail = 0;
        size_t                                           m_size = 0;
        std::unordered_map<AudioSample, Uint64>          m_lastPlayTimes;
        AudioManager                                    &m_audioManager;

        static constexpr Uint64                 MIN_REPLAY_INTERVAL = 50;
        std::unordered_map<AudioSample, Uint64> m_cooldowns;

        [[nodiscard]] bool isBufferFull() const;
        void               pushSample(QueuedSample const &queuedSample);
        void               removeExpiredSamples(Uint64 currentTime);
        void               removeAtOffset(size_t offset);
        [[nodiscard]] bool evictLowerPrioritySample(PriorityLevel incoming);
        void               trimLowPrioritySamples();

      public:
        explicit AudioSampleBuffer(AudioManager &audioManager);
        void queueSample(AudioSample   sample,
                         PriorityLevel priority);
        void update();
    };
} // namespace YerbEngine
