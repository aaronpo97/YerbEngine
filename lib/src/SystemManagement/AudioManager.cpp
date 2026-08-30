#include <SystemManagement/AudioManager.hpp>
#include <algorithm>

namespace YerbEngine {

AudioManager::AudioManager(AudioInitOptions options) {
   m_spec = SDL_AudioSpec{.format = options.format,
                          .channels = options.channels,
                          .freq = options.frequency};

   if (!SDL_Init(SDL_INIT_AUDIO)) {
      throw std::runtime_error("SDL_Init failed");
   }

   if (!MIX_Init()) {
      SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "MIX_Init error: %s",
                   SDL_GetError());
      SDL_QuitSubSystem(SDL_INIT_AUDIO);
      throw std::runtime_error("MIX_Init failed");
   }

   m_mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &m_spec);
   if (m_mixer == nullptr) {
      SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "MIX_CreateMixerDevice error: %s",
                   SDL_GetError());
      MIX_Quit();
      SDL_QuitSubSystem(SDL_INIT_AUDIO);
      throw std::runtime_error("MIX_CreateMixerDevice failed");
   }

   m_trackVoice = MIX_CreateTrack(m_mixer);
   if (m_trackVoice == nullptr) {
      SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "MIX_CreateTrack error: %s",
                   SDL_GetError());
      cleanup();
      throw std::runtime_error("MIX_CreateTrack failed");
   }

   m_sampleVoices.reserve(SAMPLE_VOICE_COUNT);
   for (size_t i = 0; i < SAMPLE_VOICE_COUNT; ++i) {
      MIX_Track* voice = MIX_CreateTrack(m_mixer);
      if (voice == nullptr) {
         SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "MIX_CreateTrack error: %s",
                      SDL_GetError());
         cleanup();
         throw std::runtime_error("MIX_CreateTrack failed");
      }
      m_sampleVoices.push_back(voice);
   }

   loadTracks(options.tracks);
   loadSamples(options.samples);
   setTrackVolume(DEFAULT_TRACK_VOLUME);
}

float AudioManager::toGain(int const volume) {
   return static_cast<float>(std::clamp(volume, 0, MAX_VOLUME)) /
          static_cast<float>(MAX_VOLUME);
}

MIX_Track* AudioManager::acquireSampleVoice() const {
   for (MIX_Track* voice : m_sampleVoices) {
      if (!MIX_TrackPlaying(voice)) {
         return voice;
      }
   }
   // Every voice is busy; reuse the oldest rather than dropping the sample.
   return m_sampleVoices.front();
}

AudioManager::~AudioManager() { cleanup(); }

void AudioManager::loadTracks(std::vector<AudioTrackDefinition> const& tracks) {
   for (auto const& track : tracks) {
      loadTrack(track.id, track.filepath);
   }
}

void AudioManager::loadSamples(
    std::vector<AudioSampleDefinition> const& samples) {
   for (auto const& sample : samples) {
      loadSample(sample.id, sample.filepath, sample.defaultVolume);
   }
}

void AudioManager::loadTrack(AudioTrackId const track, Path const& filepath) {
   AudioTrackId const key(track);
   // Tracks stream rather than predecode: they are long and play one at a
   // time.
   m_audioTracks[key] = MIX_LoadAudio(m_mixer, filepath.c_str(), false);
   if (!m_audioTracks[key]) {
      SDL_LogError(SDL_LOG_CATEGORY_AUDIO,
                   "MIX_LoadAudio error for track '%s': %s", key.c_str(),
                   SDL_GetError());
      cleanup();
      throw std::runtime_error("MIX_LoadAudio error");
   }
}

void AudioManager::loadSample(AudioSampleId const sample, Path const& filepath,
                              int const defaultVolume) {
   AudioSampleId const key(sample);
   // Samples are short and fire repeatedly, so predecode them.
   m_audioSamples[key] = MIX_LoadAudio(m_mixer, filepath.c_str(), true);
   if (!m_audioSamples[key]) {
      SDL_LogError(SDL_LOG_CATEGORY_AUDIO,
                   "MIX_LoadAudio error for sample '%s': %s", key.c_str(),
                   SDL_GetError());
      cleanup();
      throw std::runtime_error("MIX_LoadAudio error");
   }

   setSampleVolume(key, defaultVolume);
}

void AudioManager::playTrack(std::string_view const track, int const loops) {
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

      if (!MIX_SetTrackAudio(m_trackVoice, it->second)) {
         SDL_LogError(SDL_LOG_CATEGORY_AUDIO,
                      "MIX_SetTrackAudio error for track '%s': %s", key.c_str(),
                      SDL_GetError());
         return;
      }

      SDL_PropertiesID const options = SDL_CreateProperties();
      SDL_SetNumberProperty(options, MIX_PROP_PLAY_LOOPS_NUMBER, loops);
      MIX_PlayTrack(m_trackVoice, options);
      SDL_DestroyProperties(options);
   }
}

void AudioManager::playSample(std::string_view const sample, int const loops) {
   AudioSampleId const key(sample);
   auto const it = m_audioSamples.find(key);
   if (it != m_audioSamples.end() && it->second != nullptr) {
      MIX_Track* voice = acquireSampleVoice();

      if (!MIX_SetTrackAudio(voice, it->second)) {
         SDL_LogError(SDL_LOG_CATEGORY_AUDIO,
                      "MIX_SetTrackAudio error for sample '%s': %s",
                      key.c_str(), SDL_GetError());
         return;
      }

      auto const volumeIt = m_sampleVolumes.find(key);
      int const volume =
          volumeIt == m_sampleVolumes.end() ? MAX_VOLUME : volumeIt->second;
      MIX_SetTrackGain(voice, toGain(volume));

      SDL_PropertiesID const options = SDL_CreateProperties();
      SDL_SetNumberProperty(options, MIX_PROP_PLAY_LOOPS_NUMBER, loops);
      MIX_PlayTrack(voice, options);
      SDL_DestroyProperties(options);

      m_lastAudioSample = key;
   }
}

void AudioManager::stopTrack() {
   m_lastAudioTrack = m_currentAudioTrack;
   m_currentAudioTrack = {};
   MIX_StopTrack(m_trackVoice, 0);
}

void AudioManager::pauseTrack() {
   m_audioTrackPaused = true;
   MIX_PauseTrack(m_trackVoice);
}

void AudioManager::resumeTrack() {
   m_audioTrackPaused = false;
   MIX_ResumeTrack(m_trackVoice);
}

void AudioManager::setTrackVolume(int volume) {
   volume = std::clamp(volume, 0, MAX_VOLUME);
   m_trackVolume = volume;
   MIX_SetTrackGain(m_trackVoice, toGain(volume));
}

void AudioManager::setSampleVolume(std::string_view const sampleTag,
                                   int volume) {
   volume = std::clamp(volume, 0, MAX_VOLUME);

   AudioSampleId const key(sampleTag);
   if (auto const it = m_audioSamples.find(key);
       it != m_audioSamples.end() && it->second != nullptr) {
      m_sampleVolumes[key] = volume;
   }
}

int AudioManager::getSampleVolume(std::string_view const sampleTag) {
   AudioSampleId const key(sampleTag);
   auto const it = m_sampleVolumes.find(key);
   if (it == m_sampleVolumes.end()) {
      return 0;
   }
   return it->second;
}

int AudioManager::getTrackVolume() const { return m_trackVolume; }

bool AudioManager::isTrackPlaying() const {
   return MIX_TrackPlaying(m_trackVoice);
}

bool AudioManager::isTrackPaused() const {
   return MIX_TrackPaused(m_trackVoice);
}

void AudioManager::cleanup() {
   m_currentAudioTrack = {};
   m_lastAudioSample = {};

   for (MIX_Track* voice : m_sampleVoices) {
      if (voice != nullptr) {
         MIX_DestroyTrack(voice);
      }
   }
   m_sampleVoices.clear();

   if (m_trackVoice != nullptr) {
      MIX_DestroyTrack(m_trackVoice);
      m_trackVoice = nullptr;
   }

   for (auto& [sampleTag, sample] : m_audioSamples) {
      if (sample != nullptr) {
         SDL_LogInfo(SDL_LOG_CATEGORY_AUDIO, "Freeing audio sample '%s'",
                     sampleTag.c_str());
         MIX_DestroyAudio(sample);
         sample = nullptr;
      }
   }

   for (auto& [trackTag, track] : m_audioTracks) {
      if (track != nullptr) {
         SDL_LogInfo(SDL_LOG_CATEGORY_AUDIO, "Freeing audio track '%s'",
                     trackTag.c_str());
         MIX_DestroyAudio(track);
         track = nullptr;
      }
   }

   if (m_mixer != nullptr) {
      MIX_DestroyMixer(m_mixer);
      m_mixer = nullptr;
   }

   MIX_Quit();

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
      setTrackVolume(0);
      m_tracksMuted = true;
   }
}

void AudioManager::unmuteTracks() {
   if (m_tracksMuted) {
      setTrackVolume(m_savedTrackVolume);
      m_tracksMuted = false;
   }
}

void AudioManager::muteSamples() {
   if (m_samplesMuted) {
      return;
   }

   for (auto const& [sampleTag, sample] : m_audioSamples) {
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

   for (auto const& [sampleTag, audioSample] : m_audioSamples) {
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

}  // namespace YerbEngine
