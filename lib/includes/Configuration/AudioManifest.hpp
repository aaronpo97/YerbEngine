#pragma once

#include <SystemManagement/AudioManager.hpp>
#include <filesystem>
#include <unordered_map>

namespace YerbEngine {

/** The audio a game loads at startup, as described by its manifest. */
struct AudioManifest {
   AudioInitOptions options;
   std::unordered_map<AudioSampleId, Uint64> sampleCooldowns;
};

/**
 * Reads an audio manifest: the tracks and samples a game loads, the volume
 * each sample plays at, and how long a sample waits before it may replay.
 *
 * The manifest is JSON with a "tracks" and a "samples" array:
 *
 *     {
 *       "tracks":  [ { "id": "track_play", "file": "audio/tracks/play.ogg" } ],
 *       "samples": [ { "id": "sample_shoot",
 *                      "file": "audio/samples/shoot.wav",
 *                      "volume": 32,
 *                      "cooldown": 100 } ]
 *     }
 *
 * Paths are relative to assetsDir. "volume" is on the 0 to MAX_VOLUME
 * scale and defaults to AudioManager::DEFAULT_SAMPLE_VOLUME; "cooldown" is
 * in milliseconds and may be left out.
 *
 * @throws std::runtime_error if the manifest exists but cannot be parsed.
 */
AudioManifest loadAudioManifest(std::filesystem::path const& manifestPath,
                                std::filesystem::path const& assetsDir);

}  // namespace YerbEngine
