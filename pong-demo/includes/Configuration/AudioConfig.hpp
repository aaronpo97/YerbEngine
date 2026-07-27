#pragma once

#include <Configuration/AudioIds.hpp>
#include <SDL.h>
#include <SystemManagement/AudioManager.hpp>
#include <filesystem>
#include <unordered_map>

namespace PongAudio {

    YerbEngine::AudioInitOptions
    buildAudioInitOptions(std::filesystem::path const &assetsDir);

    std::unordered_map<YerbEngine::AudioSampleId,
                       Uint64>
    sampleCooldowns();

} // namespace PongAudio
