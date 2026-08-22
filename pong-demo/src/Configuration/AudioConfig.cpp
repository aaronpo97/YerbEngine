#include <Configuration/AudioConfig.hpp>

namespace PongAudio {

    YerbEngine::AudioInitOptions
    buildAudioInitOptions(std::filesystem::path const &assetsDir) {
        using YerbEngine::AudioSampleDefinition;

        YerbEngine::AudioInitOptions options{};
        std::filesystem::path const  samplesDir =
            assetsDir / "audio" / "samples";

        options.samples = {
            {std::string(SAMPLE_PADDLE_HIT), samplesDir / "paddle_hit.wav",
             YerbEngine::AudioManager::DEFAULT_SAMPLE_VOLUME},
            {std::string(SAMPLE_WALL_BOUNCE), samplesDir / "wall_bounce.wav",
             YerbEngine::AudioManager::DEFAULT_SAMPLE_VOLUME},
            {std::string(SAMPLE_SCORE), samplesDir / "score.wav",
             YerbEngine::AudioManager::DEFAULT_SAMPLE_VOLUME},
            {std::string(SAMPLE_MENU_MOVE), samplesDir / "menu_move.wav",
             YerbEngine::AudioManager::DEFAULT_SAMPLE_VOLUME},
            {std::string(SAMPLE_MENU_SELECT), samplesDir / "menu_select.wav",
             YerbEngine::AudioManager::DEFAULT_SAMPLE_VOLUME},
        };

        return options;
    }

    std::unordered_map<YerbEngine::AudioSampleId,
                       Uint64>
    sampleCooldowns() {
        using YerbEngine::AudioSampleId;

        return {
            {AudioSampleId(SAMPLE_PADDLE_HIT), 60},
            {AudioSampleId(SAMPLE_WALL_BOUNCE), 60},
            {AudioSampleId(SAMPLE_SCORE), 200},
        };
    }

} // namespace PongAudio
