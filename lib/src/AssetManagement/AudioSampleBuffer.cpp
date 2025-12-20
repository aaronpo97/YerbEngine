#include <AssetManagement/AudioSampleBuffer.hpp>

namespace YerbEngine {

    AudioSampleBuffer::AudioSampleBuffer(AudioManager &audioManager)
        : m_audioManager(audioManager),
          m_cooldowns{
              {AudioSample::SHOOT,
               100},
              {AudioSample::ENEMY_COLLISION,
               200},
              {AudioSample::ITEM_ACQUIRED,
               150},
              {AudioSample::SPEED_BOOST,
               150},
              {AudioSample::SLOWNESS_DEBUFF,
               150},
              {AudioSample::BULLET_HIT_01,
               100},
              {AudioSample::BULLET_HIT_02,
               100},
          } {}

    void AudioSampleBuffer::queueSample(AudioSample const   sample,
                                        PriorityLevel const priority) {
        Uint64 const currentTime = SDL_GetTicks64();
        removeExpiredSamples(currentTime);

        auto const lastPlayIt = m_lastPlayTimes.find(sample);
        if (lastPlayIt != m_lastPlayTimes.end()) {
            Uint64 const timeSinceLastPlay = currentTime - lastPlayIt->second;
            Uint64       cooldown          = MIN_REPLAY_INTERVAL;
            if (auto const cooldownIt = m_cooldowns.find(sample);
                cooldownIt != m_cooldowns.end()) {
                cooldown = cooldownIt->second;
            }
            if (timeSinceLastPlay < cooldown) {
                return;
            }
        }

        if (isBufferFull() && !evictLowerPrioritySample(priority)) {
            return;
        }

        pushSample(
            {.sample = sample, .priority = priority, .timestamp = currentTime});
    }

    void AudioSampleBuffer::update() {
        Uint64 const     currentTime           = SDL_GetTicks64();
        size_t           soundsPlayedThisFrame = 0;
        constexpr size_t MAX_SOUNDS_PER_FRAME =
            AudioManager::MAX_SAMPLES_PER_FRAME;

        removeExpiredSamples(currentTime);

        while (m_size > 0 && soundsPlayedThisFrame < MAX_SOUNDS_PER_FRAME) {
            bool        foundBest  = false;
            size_t      bestOffset = 0;
            QueuedSample bestSample{};

            size_t index = m_head;
            for (size_t offset = 0; offset < m_size; ++offset) {
                QueuedSample const &candidate = m_sampleBuffer[index];
                if (!foundBest ||
                    candidate.priority > bestSample.priority ||
                    (candidate.priority == bestSample.priority &&
                     candidate.timestamp < bestSample.timestamp)) {
                    bestOffset = offset;
                    bestSample = candidate;
                    foundBest  = true;
                }
                index = (index + 1) % SAMPLE_BUFFER_CAPACITY;
            }

            if (!foundBest) {
                break;
            }

            m_audioManager.playSample(bestSample.sample);
            m_lastPlayTimes[bestSample.sample] = currentTime;
            removeAtOffset(bestOffset);
            soundsPlayedThisFrame += 1;
        }

        trimLowPrioritySamples();
    }

    bool AudioSampleBuffer::isBufferFull() const {
        return m_size == SAMPLE_BUFFER_CAPACITY;
    }

    void AudioSampleBuffer::pushSample(QueuedSample const &queuedSample) {
        m_sampleBuffer[m_tail] = queuedSample;
        m_tail = (m_tail + 1) % SAMPLE_BUFFER_CAPACITY;
        if (m_size < SAMPLE_BUFFER_CAPACITY) {
            m_size += 1;
        } else {
            m_head = (m_head + 1) % SAMPLE_BUFFER_CAPACITY;
        }
    }

    void AudioSampleBuffer::removeExpiredSamples(Uint64 currentTime) {
        bool   removed = false;
        size_t index   = m_head;

        std::array<QueuedSample, SAMPLE_BUFFER_CAPACITY> newBuffer{};
        size_t                                           newSize = 0;

        for (size_t i = 0; i < m_size; ++i) {
            QueuedSample const &sample = m_sampleBuffer[index];
            if (currentTime - sample.timestamp > STALE_SAMPLE_MS) {
                removed = true;
            } else {
                newBuffer[newSize++] = sample;
            }
            index = (index + 1) % SAMPLE_BUFFER_CAPACITY;
        }

        if (!removed) {
            return;
        }

        m_sampleBuffer = newBuffer;
        m_head         = 0;
        m_tail         = newSize % SAMPLE_BUFFER_CAPACITY;
        m_size         = newSize;
    }

    void AudioSampleBuffer::removeAtOffset(size_t offset) {
        if (offset >= m_size) {
            return;
        }

        std::array<QueuedSample, SAMPLE_BUFFER_CAPACITY> newBuffer{};
        size_t                                           newSize = 0;
        size_t                                           index   = m_head;

        for (size_t i = 0; i < m_size; ++i) {
            if (i != offset) {
                newBuffer[newSize++] = m_sampleBuffer[index];
            }
            index = (index + 1) % SAMPLE_BUFFER_CAPACITY;
        }

        m_sampleBuffer = newBuffer;
        m_head         = 0;
        m_tail         = newSize % SAMPLE_BUFFER_CAPACITY;
        m_size         = newSize;
    }

    bool AudioSampleBuffer::evictLowerPrioritySample(
        PriorityLevel const incoming) {
        bool   removed = false;
        size_t index   = m_head;

        std::array<QueuedSample, SAMPLE_BUFFER_CAPACITY> newBuffer{};
        size_t                                           newSize = 0;

        for (size_t i = 0; i < m_size; ++i) {
            QueuedSample const &sample = m_sampleBuffer[index];
            if (!removed && sample.priority < incoming) {
                removed = true;
            } else {
                newBuffer[newSize++] = sample;
            }
            index = (index + 1) % SAMPLE_BUFFER_CAPACITY;
        }

        if (!removed) {
            return false;
        }

        m_sampleBuffer = newBuffer;
        m_head         = 0;
        m_tail         = newSize % SAMPLE_BUFFER_CAPACITY;
        m_size         = newSize;
        return true;
    }

    void AudioSampleBuffer::trimLowPrioritySamples() {
        if (m_size <= TRIM_LIMIT) {
            return;
        }

        bool   removed = false;
        size_t index   = m_head;

        std::array<QueuedSample, SAMPLE_BUFFER_CAPACITY> newBuffer{};
        size_t                                           newSize = 0;

        for (size_t i = 0; i < m_size; ++i) {
            QueuedSample const &sample = m_sampleBuffer[index];
            if (sample.priority >= PriorityLevel::STANDARD) {
                newBuffer[newSize++] = sample;
            } else {
                removed = true;
            }
            index = (index + 1) % SAMPLE_BUFFER_CAPACITY;
        }

        if (!removed) {
            return;
        }

        m_sampleBuffer = newBuffer;
        m_head         = 0;
        m_tail         = newSize % SAMPLE_BUFFER_CAPACITY;
        m_size         = newSize;
    }

} // namespace YerbEngine
