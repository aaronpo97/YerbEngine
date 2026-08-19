#pragma once

#include <Configuration/AudioIds.hpp>
#include <SystemManagement/AudioManager.hpp>
#include <SDL.h>
#include <filesystem>
#include <unordered_map>
#include <vector>

namespace DemoAudio {

    YerbEngine::AudioInitOptions
    buildAudioInitOptions(std::filesystem::path const &assetsDir);

    std::unordered_map<YerbEngine::AudioSampleId, Uint64> sampleCooldowns();

} // namespace DemoAudio
