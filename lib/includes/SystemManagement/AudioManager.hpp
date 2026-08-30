#pragma once

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

#include <filesystem>
#include <map>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace YerbEngine {

typedef std::filesystem::path Path;

using AudioSampleId = std::string;
using AudioTrackId = std::string;

struct AudioTrackDefinition {
   AudioTrackId id;
   Path filepath;
};

/**
 * The inclusive upper bound of every volume value in this API.
 *
 * SDL_mixer works in float gain; AudioManager converts internally so that
 * callers keep a single integer scale.
 */
constexpr int MAX_VOLUME = 128;

struct AudioSampleDefinition {
   AudioSampleId id;
   Path filepath;
   int defaultVolume = MAX_VOLUME;
};

struct AudioInitOptions {
   int frequency = 44100;
   SDL_AudioFormat format = SDL_AUDIO_F32;
   int channels = 2;
   std::vector<AudioTrackDefinition> tracks{};
   std::vector<AudioSampleDefinition> samples{};
};

class AudioManager {
   MIX_Mixer* m_mixer = nullptr;

   std::unordered_map<AudioTrackId, MIX_Audio*> m_audioTracks;
   std::unordered_map<AudioSampleId, MIX_Audio*> m_audioSamples;

   /*
    * SDL_mixer carries gain on the track rather than on the audio, so the
    * per-sample volumes the API exposes are kept here and applied when a
    * sample is bound to a voice.
    */
   std::unordered_map<AudioSampleId, int> m_sampleVolumes;

   /// Dedicated voice for the looping background track.
   MIX_Track* m_trackVoice = nullptr;

   /*
    * Fixed pool of voices for samples, standing in for SDL_mixer 2's
    * automatic channel allocation. Sized above MAX_SAMPLES_PER_FRAME so a
    * busy frame cannot cut off the previous frame's samples.
    */
   static constexpr size_t SAMPLE_VOICE_COUNT = 16;
   std::vector<MIX_Track*> m_sampleVoices;

   SDL_AudioSpec m_spec{};

   AudioTrackId m_currentAudioTrack{};
   AudioTrackId m_lastAudioTrack{};
   AudioSampleId m_lastAudioSample{};
   bool m_audioTrackPaused = false;

   bool m_tracksMuted = false;
   bool m_samplesMuted = false;
   int m_savedTrackVolume = MAX_VOLUME;
   int m_trackVolume = MAX_VOLUME;

   std::map<AudioSampleId, int> m_savedSampleVolumes;

   /**
    * Converts a volume on this API's 0 to MAX_VOLUME scale into the
    * 0.0 to 1.0 gain SDL_mixer expects.
    */
   static float toGain(int volume);

   /**
    * Finds a free voice in the sample pool, falling back to the first
    * voice when every one of them is busy.
    */
   MIX_Track* acquireSampleVoice() const;

   /**
    * Loads an audio track from a file.
    *
    * @param track The audio track to load.
    * @param filepath The path to the audio file.
    *
    * @throws std::runtime_error if the file could not be loaded.
    */
   void loadTrack(AudioTrackId const track, Path const& filepath);

   /**
    * Loads an audio sample from a file.
    *
    * @param sample The audio sample to load.
    * @param filepath The path to the audio file.
    *
    * @throws std::runtime_error if the file could not be loaded.
    */
   void loadSample(AudioSampleId sample, Path const& filepath,
                   int defaultVolume);

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
    * @throws std::runtime_error if SDL_Init(SDL_INIT_AUDIO) or the
    * mixer device fails to open.
    *
    * @param frequency The frequency to playback audio at (in Hz).
    * @param format Audio format, one of SDL's SDL_AUDIO_* values.
    * @param channels number of channels.
    */
   AudioManager() : AudioManager(AudioInitOptions{}) {}

   explicit AudioManager(AudioInitOptions options);

   static constexpr size_t MAX_SAMPLES_PER_FRAME = 4;
   static constexpr int DEFAULT_SAMPLE_VOLUME =
       MAX_VOLUME / MAX_SAMPLES_PER_FRAME;
   static constexpr int DEFAULT_TRACK_VOLUME = MAX_VOLUME * 0.8;

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
   void loadTracks(std::vector<AudioTrackDefinition> const& tracks);
   void loadSamples(std::vector<AudioSampleDefinition> const& samples);

   /**
    * Plays an audio track.
    *
    * If the track is already playing, it will be ignored.
    *
    * @param track The audio track to play.
    * @param loops The number of times to loop the track (-1 for infinite).
    */
   void playTrack(std::string_view track, int loops = -1);

   /**
    * Plays an audio sample.
    *
    * @param sample The audio sample to play.
    * @param loops The number of times to loop the sample (0 for no loop).
    */
   void playSample(std::string_view sample, int loops = 0);

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
    * @param volume The volume level (0 to MAX_VOLUME).
    */
   void setTrackVolume(int volume);

   /**
    * Sets the volume of a specific audio sample.
    *
    * @param sampleTag The audio sample to set the volume for.
    * @param volume The volume level (0 to MAX_VOLUME).
    */
   void setSampleVolume(std::string_view sampleTag, int volume);

   /**
    * Gets the volume of a specific audio sample.
    *
    * @param sampleTag The audio sample to get the volume for.
    * @return The volume level (0 to MAX_VOLUME).
    */
   int getSampleVolume(std::string_view sampleTag);

   /**
    * Gets the volume of the currently playing audio track.
    *
    * @return The volume level (0 to MAX_VOLUME).
    */
   int getTrackVolume() const;

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
    * Checks whether the audio track is contributing to the mix.
    *
    * A paused track reports false here and true from isTrackPaused().
    *
    * @return true if the track is playing.
    */
   [[nodiscard]] bool isTrackPlaying() const;

   /**
    * Checks whether the audio track is paused.
    *
    * @return true if the track is paused.
    */
   [[nodiscard]] bool isTrackPaused() const;
};

}  // namespace YerbEngine
