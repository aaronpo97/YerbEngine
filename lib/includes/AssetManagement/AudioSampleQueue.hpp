#pragma once

#include <SystemManagement/AudioManager.hpp>
#include <queue>
#include <unordered_map>
namespace YerbEngine {enum class PriorityLevel : uint8_t {
    BACKGROUND,
    STANDARD,
    IMPORTANT,
    CRITICAL
};
class AudioSampleQueue {

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

    std::priority_queue<QueuedSample>       m_sampleQueue;
    std::unordered_map<AudioSample, Uint64> m_lastPlayTimes;
    AudioManager                           &m_audioManager;

    static constexpr Uint64                 MIN_REPLAY_INTERVAL = 50;
    std::unordered_map<AudioSample, Uint64> m_cooldowns;

  public:
    explicit AudioSampleQueue(AudioManager &audioManager);
    void queueSample(AudioSample sample, PriorityLevel priority);
    void update();
};}