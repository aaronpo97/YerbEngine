#include <SystemManagement/AudioManager.hpp>

namespace YerbEngine {

    AudioManager::AudioManager(AudioInitOptions options)
        : m_frequency(options.frequency),
          m_format(options.format),
          m_channels(options.channels),
          m_chunksize(options.chunksize) {
        if (SDL_Init(SDL_INIT_AUDIO) != 0) {
            throw std::runtime_error("SDL_Init failed");
        }

        if (Mix_OpenAudio(m_frequency, m_format, m_channels, m_chunksize) !=
            0) {
            SDL_Quit();
            throw std::runtime_error("Mix_OpenAudio failed");
        }

        loadTracks(options.tracks);
        loadSamples(options.samples);
        setTrackVolume(DEFAULT_TRACK_VOLUME);
    }

    AudioManager::~AudioManager() { cleanup(); }

    void AudioManager::loadTracks(
        std::vector<AudioTrackDefinition> const &tracks) {
        for (auto const &track : tracks) {
            loadTrack(track.id, track.filepath);
        }
    }

    void AudioManager::loadSamples(
        std::vector<AudioSampleDefinition> const &samples) {
        for (auto const &sample : samples) {
            loadSample(sample.id, sample.filepath, sample.defaultVolume);
        }
    }

    void AudioManager::loadTrack(AudioTrackId const track,
                                 Path const        &filepath) {
        AudioTrackId const key(track);
        m_audioTracks[key] = Mix_LoadMUS(filepath.c_str());
        if (!m_audioTracks[key]) {
            SDL_LogError(SDL_LOG_CATEGORY_AUDIO,
                         "Mix_LoadMUS error for '%s': %s", key.c_str(),
                         Mix_GetError());
            cleanup();
            throw std::runtime_error("Mix_LoadMUS error");
        }
    }

    void AudioManager::loadSample(AudioSampleId const sample,
                                  Path const         &filepath,
                                  int const           defaultVolume) {
        AudioSampleId const key(sample);
        m_audioSamples[key] = Mix_LoadWAV(filepath.c_str());
        if (!m_audioSamples[key]) {
            SDL_LogError(SDL_LOG_CATEGORY_AUDIO,
                         "Mix_LoadWAV error for '%s': %s", key.c_str(),
                         Mix_GetError());
            cleanup();
            throw std::runtime_error("Mix_LoadWAV error");
        }

        setSampleVolume(key, defaultVolume);
    }

    void AudioManager::playTrack(std::string_view const track,
                                 int const              loops) {
        AudioTrackId const key(track);
        if (key == m_currentAudioTrack) {
            SDL_LogInfo(SDL_LOG_CATEGORY_AUDIO,
                        "Track '%s' is already playing, ignoring request.",
                        key.c_str());
            return;
        }

        m_lastAudioTrack = m_currentAudioTrack;
        auto const it = m_audioTracks.find(key);
        if (it != m_audioTracks.end() && it->second != nullptr) {
            m_currentAudioTrack = key;
            Mix_PlayMusic(it->second, loops);
        }
    }

    void AudioManager::playSample(std::string_view const sample,
                                  int const              loops) {
        AudioSampleId const key(sample);
        auto const          it = m_audioSamples.find(key);
        if (it != m_audioSamples.end() && it->second != nullptr) {
            Mix_PlayChannel(-1, it->second, loops);
            m_lastAudioSample = key;
        }
    }

    void AudioManager::stopTrack() {
        m_lastAudioTrack    = m_currentAudioTrack;
        m_currentAudioTrack = {};
        Mix_HaltMusic();
    }

    void AudioManager::pauseTrack() {
        m_audioTrackPaused = true;
        Mix_PauseMusic();
    }

    void AudioManager::resumeTrack() {
        m_audioTrackPaused = false;
        Mix_ResumeMusic();
    }

    void AudioManager::setTrackVolume(int volume) {
        if (volume > MIX_MAX_VOLUME) {
            volume = MIX_MAX_VOLUME;
        }

        if (volume < 0) {
            volume = 0;
        }

        Mix_VolumeMusic(volume);
    }

    void AudioManager::setSampleVolume(std::string_view const sampleTag,
                                       int                    volume) {
        if (volume > MIX_MAX_VOLUME) {
            volume = MIX_MAX_VOLUME;
        }

        if (volume < 0) {
            volume = 0;
        }

        AudioSampleId const key(sampleTag);
        if (auto const it = m_audioSamples.find(key);
            it != m_audioSamples.end() && it->second != nullptr) {
            Mix_VolumeChunk(it->second, volume);
        }
    }

    int AudioManager::getSampleVolume(std::string_view const sampleTag) {
        AudioSampleId const key(sampleTag);
        auto const          it = m_audioSamples.find(key);
        if (it == m_audioSamples.end() || it->second == nullptr) {
            return 0;
        }
        // Use -1 to query for the current sample volume.
        return Mix_VolumeChunk(it->second, -1);
    }

    int AudioManager::getTrackVolume() {
        // Use -1 to query for the current track volume.
        return Mix_VolumeMusic(-1);
    }

    bool AudioManager::isTrackPlaying() { return Mix_PlayingMusic() == 1; }

    bool AudioManager::isTrackPaused() { return Mix_PausedMusic() == 1; }

    void AudioManager::cleanup() {
        m_currentAudioTrack = {};
        m_lastAudioSample   = {};

        for (auto &[sampleTag, sample] : m_audioSamples) {
            if (sample != nullptr) {
                SDL_LogInfo(SDL_LOG_CATEGORY_AUDIO,
                            "Freeing audio sample '%s'", sampleTag.c_str());
                Mix_FreeChunk(sample);
                sample = nullptr;
            }
        }

        for (auto &[trackTag, track] : m_audioTracks) {
            if (track != nullptr) {
                SDL_LogInfo(SDL_LOG_CATEGORY_AUDIO,
                            "Freeing audio track '%s'", trackTag.c_str());
                Mix_FreeMusic(track);
                track = nullptr;
            }
        }

        Mix_CloseAudio();
        Mix_Quit();

        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                    "AudioManager cleaned up successfully!");
    }

    AudioTrackId AudioManager::getCurrentAudioTrack() const {
        return m_currentAudioTrack;
    }

    AudioTrackId AudioManager::getLastAudioTrack() const {
        return m_lastAudioTrack;
    }

    AudioSampleId AudioManager::getLastAudioSample() const {
        return m_lastAudioSample;
    }

    void AudioManager::muteTracks() {
        if (!m_tracksMuted) {
            // -1 queries for the current volume
            m_savedTrackVolume = getTrackVolume();
            Mix_VolumeMusic(0);
            m_tracksMuted = true;
        }
    }

    void AudioManager::unmuteTracks() {
        if (m_tracksMuted) {
            Mix_VolumeMusic(m_savedTrackVolume);
            m_tracksMuted = false;
        }
    }

    void AudioManager::muteSamples() {
        if (m_samplesMuted) {
            return;
        }

        for (auto const &[sampleTag, sample] : m_audioSamples) {
            if (sample != nullptr) {
                m_savedSampleVolumes[sampleTag] = getSampleVolume(sampleTag);
                setSampleVolume(sampleTag, 0);
            }
        }

        m_samplesMuted = true;
    }

    void AudioManager::unmuteSamples() {
        if (!m_samplesMuted) {
            return;
        }

        for (auto const &[sampleTag, audioSample] : m_audioSamples) {
            if (audioSample != nullptr) {
                int const savedVolume = m_savedSampleVolumes[sampleTag];
                setSampleVolume(sampleTag, savedVolume);
            }
        }
        m_samplesMuted = false;
    }

    void AudioManager::muteAll() {
        muteTracks();
        muteSamples();
    }

    void AudioManager::unmuteAll() {
        unmuteTracks();
        unmuteSamples();
    }

    void AudioManager::toggleMuteAll() {

        if (m_tracksMuted || m_samplesMuted) {
            unmuteTracks();
            unmuteSamples();
        } else {
            muteTracks();
            muteSamples();
        }
    }

    void AudioManager::toggleMuteTracks() {
        m_tracksMuted ? unmuteTracks() : muteTracks();
    }

    void AudioManager::toggleMuteSamples() {
        m_samplesMuted ? unmuteSamples() : muteSamples();
    }

} // namespace YerbEngine
