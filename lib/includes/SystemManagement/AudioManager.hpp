#pragma once

#include <SDL.h>
#include <SDL_mixer.h>
#include <filesystem>
#include <map>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace YerbEngine {

    typedef std::filesystem::path Path;

    using AudioSampleId = std::string;
    using AudioTrackId  = std::string;

    struct AudioTrackDefinition {
        AudioTrackId id;
        Path         filepath;
    };

    struct AudioSampleDefinition {
        AudioSampleId id;
        Path          filepath;
        int           defaultVolume = MIX_MAX_VOLUME;
    };

    struct AudioInitOptions {
        int frequency = 44100;
        Uint16 format = MIX_DEFAULT_FORMAT;
        int channels = 2;
        int chunksize = 2048;
        std::vector<AudioTrackDefinition>  tracks{};
        std::vector<AudioSampleDefinition> samples{};
    };

    class AudioManager {
        std::unordered_map<AudioTrackId, Mix_Music *>  m_audioTracks;
        std::unordered_map<AudioSampleId, Mix_Chunk *> m_audioSamples;

        int    m_frequency{};
        Uint16 m_format{};
        int    m_channels{};
        int    m_chunksize{};

        AudioTrackId  m_currentAudioTrack{};
        AudioTrackId  m_lastAudioTrack{};
        AudioSampleId m_lastAudioSample{};
        bool          m_audioTrackPaused = false;

        bool                       m_tracksMuted      = false;
        bool                       m_samplesMuted     = false;
        int                        m_savedTrackVolume = MIX_MAX_VOLUME;
        std::map<AudioSampleId, int> m_savedSampleVolumes;

        /**
         * Loads an audio track from a file.
         *
         * @param track The audio track to load.
         * @param filepath The path to the audio file.
         *
         * @throws std::runtime_error if Mix_LoadMUS fails.
         */
        void loadTrack(AudioTrackId const track,
                       Path const &filepath);

        /**
         * Loads an audio sample from a file.
         *
         * @param sample The audio sample to load.
         * @param filepath The path to the audio file.
         *
         * @throws std::runtime_error if Mix_LoadWAV fails.
         */
        void loadSample(AudioSampleId sample,
                        Path const   &filepath,
                        int           defaultVolume);

        /**
         * Cleans up the audio resources and closes the audio subsystem.
         *
         * This function frees all loaded audio tracks and samples, stops any
         * currently playing track, and closes the audio subsystem.
         */
        void cleanup();

      public:
        /**
         *
         * Constructor for AudioManager.
         *
         * Initializes SDL audio subsystem and loads all audio tracks and
         * samples.
         *
         * @throws std::runtime_error if SDL_Init(SDL_INIT_AUDIO) or
         * Mix_OpenAudio fails.
         *
         * @param frequency The frequency to playback audio at (in Hz).
         * @param format Audio format, one of SDL's AUDIO_* values.
         * @param channels number of channels.
         * @param chunksize Audio buffer size in sample FRAMES (total samples
         * divided by channel count).
         */
        AudioManager()
            : AudioManager(AudioInitOptions{}) {}

        explicit AudioManager(AudioInitOptions options);

        static constexpr size_t MAX_SAMPLES_PER_FRAME = 4;
        static constexpr int    DEFAULT_SAMPLE_VOLUME =
            MIX_MAX_VOLUME / MAX_SAMPLES_PER_FRAME;
        static constexpr int DEFAULT_TRACK_VOLUME = MIX_MAX_VOLUME * 0.8;

        /**
         * Destructor for AudioManager.
         *
         * Calls cleanup() to free audio resources and close the audio
         * subsystem.
         */
        ~AudioManager();

        /**
         * Loads all audio tracks and samples.
         *
         * This function loads all audio tracks and samples into the
         * AudioManager using the provided definitions.
         */
        void loadTracks(std::vector<AudioTrackDefinition> const &tracks);
        void
        loadSamples(std::vector<AudioSampleDefinition> const &samples);

        /**
         * Plays an audio track.
         *
         * If the track is already playing, it will be ignored.
         *
         * @param track The audio track to play.
         * @param loops The number of times to loop the track (-1 for infinite).
         */
        void playTrack(std::string_view track,
                       int              loops = -1);

        /**
         * Plays an audio sample.
         *
         * @param sample The audio sample to play.
         * @param loops The number of times to loop the sample (0 for no loop).
         */
        void playSample(std::string_view sample,
                        int             loops = 0);

        /**
         * Stops the currently playing audio track.
         *
         * This function stops the currently playing audio track and sets the
         * current audio track to NONE.
         */
        void stopTrack();

        /**
         * Pauses the currently playing audio track.
         *
         * This function pauses the currently playing audio track and sets the
         * m_audioTrackPaused flag to true.
         */
        void pauseTrack();

        /**
         * Resumes the currently paused audio track.
         *
         * This function resumes the currently paused audio track and sets the
         * m_audioTrackPaused flag to false.
         */
        void resumeTrack();

        /**
         * Sets the volume of the currently playing audio track.
         *
         * @param volume The volume level (0 to MIX_MAX_VOLUME).
         */
        static void setTrackVolume(int volume);

        /**
         * Sets the volume of a specific audio sample.
         *
         * @param sampleTag The audio sample to set the volume for.
         * @param volume The volume level (0 to MIX_MAX_VOLUME).
         */
        void setSampleVolume(std::string_view sampleTag,
                             int             volume);

        /**
         * Gets the volume of a specific audio sample.
         *
         * @param sampleTag The audio sample to get the volume for.
         * @return The volume level (0 to MIX_MAX_VOLUME).
         */
        int getSampleVolume(std::string_view sampleTag);

        /**
         * Gets the volume of the currently playing audio track.
         *
         * @return The volume level (0 to MIX_MAX_VOLUME).
         */
        static int getTrackVolume();

        /**
         * Mutes all audio tracks.
         *
         * This function sets the volume of all audio tracks to 0 and stores the
         * current volume level for later restoration.
         */
        void muteTracks();

        /**
         * Unmutes all audio tracks.
         *
         * This function restores the volume of all audio tracks to their
         * previous level.
         *
         *
         */
        void unmuteTracks();

        /**
         * Mutes all audio samples.
         *
         * This function sets the volume of all audio samples to 0 and stores
         * the current volume level for later restoration.
         */
        void muteSamples();

        /**
         * Unmutes all audio samples.
         *
         * This function restores the volume of all audio samples to their
         * previous level.
         */
        void unmuteSamples();

        /**
         * Toggles the mute state of all audio tracks and samples.
         *
         * If any track or sample is muted, it will be unmuted, and vice versa.
         */
        void muteAll();

        /**
         * Unmutes all audio tracks and samples.
         *
         * This function restores the volume of all audio tracks and samples to
         * their previous level.
         */
        void unmuteAll();

        /**
         * Toggles the mute state of all audio tracks and samples.
         *
         * If any track or sample is muted, it will be unmuted, and vice versa.
         */
        void toggleMuteAll();

        /**
         * Toggles the mute state of all audio tracks.
         *
         * If any track is muted, it will be unmuted, and vice versa.
         */
        void toggleMuteTracks();

        /**
         * Toggles the mute state of all audio samples.
         *
         * If any sample is muted, it will be unmuted, and vice versa.
         */
        void toggleMuteSamples();

        /**
         * Get the current audio track.
         *
         * @return AudioTrack The current audio track.
         */
        AudioTrackId getCurrentAudioTrack() const;

        /**
         * Get the last played audio track.
         *
         * @return AudioTrack The last played audio track.
         */
        AudioTrackId getLastAudioTrack() const;

        /**
         * Get the last played audio sample.
         *
         * @return AudioSample The last played audio sample.
         */
        AudioSampleId getLastAudioSample() const;

        /**
         * Checks if the currently playing audio track is paused.
         *
         * @return true if the track is paused, false otherwise.
         */

        [[nodiscard]] static bool isTrackPlaying();

        /**
         * Checks if the currently playing audio track is paused.
         *
         * @return true if the track is paused, false otherwise.
         */
        [[nodiscard]] static bool isTrackPaused();
    };

} // namespace YerbEngine
