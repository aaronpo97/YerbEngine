#include <Configuration/AudioConfig.hpp>

namespace DemoAudio {

    YerbEngine::AudioInitOptions
    buildAudioInitOptions(std::filesystem::path const &assetsDir) {
        using YerbEngine::AudioSampleDefinition;
        using YerbEngine::AudioTrackDefinition;

        YerbEngine::AudioInitOptions options{};
        std::filesystem::path const  tracksDir = assetsDir / "audio" / "tracks";
        std::filesystem::path const  samplesDir =
            assetsDir / "audio" / "samples";

        options.tracks = {
            {std::string(TRACK_MAIN_MENU), tracksDir / "main_menu.ogg"},
            {std::string(TRACK_PLAY), tracksDir / "play.ogg"},
        };

        options.samples = {
            {std::string(SAMPLE_ITEM_ACQUIRED), samplesDir / "item_acquired.wav",
             YerbEngine::AudioManager::DEFAULT_SAMPLE_VOLUME},
            {std::string(SAMPLE_ENEMY_COLLISION),
             samplesDir / "enemy_collides.wav",
             YerbEngine::AudioManager::DEFAULT_SAMPLE_VOLUME},
            {std::string(SAMPLE_SPEED_BOOST),
             samplesDir / "speed_boost_acquired.wav",
             YerbEngine::AudioManager::DEFAULT_SAMPLE_VOLUME},
            {std::string(SAMPLE_SLOWNESS_DEBUFF),
             samplesDir / "slowness_debuff_acquired.wav",
             YerbEngine::AudioManager::DEFAULT_SAMPLE_VOLUME},
            {std::string(SAMPLE_MENU_MOVE), samplesDir / "menu_move.wav",
             YerbEngine::AudioManager::DEFAULT_SAMPLE_VOLUME},
            {std::string(SAMPLE_MENU_SELECT), samplesDir / "menu_select.wav",
             YerbEngine::AudioManager::DEFAULT_SAMPLE_VOLUME},
            {std::string(SAMPLE_SHOOT), samplesDir / "shoot.wav",
             YerbEngine::AudioManager::DEFAULT_SAMPLE_VOLUME},
            {std::string(SAMPLE_BULLET_HIT_01),
             samplesDir / "bullet_hit_01.wav",
             YerbEngine::AudioManager::DEFAULT_SAMPLE_VOLUME / 2},
            {std::string(SAMPLE_BULLET_HIT_02),
             samplesDir / "bullet_hit_02.wav",
             YerbEngine::AudioManager::DEFAULT_SAMPLE_VOLUME},
        };

        return options;
    }

    std::unordered_map<YerbEngine::AudioSampleId, Uint64> sampleCooldowns() {
        using YerbEngine::AudioSampleId;

        return {
            {AudioSampleId(SAMPLE_SHOOT), 100},
            {AudioSampleId(SAMPLE_ENEMY_COLLISION), 200},
            {AudioSampleId(SAMPLE_ITEM_ACQUIRED), 150},
            {AudioSampleId(SAMPLE_SPEED_BOOST), 150},
            {AudioSampleId(SAMPLE_SLOWNESS_DEBUFF), 150},
            {AudioSampleId(SAMPLE_BULLET_HIT_01), 100},
            {AudioSampleId(SAMPLE_BULLET_HIT_02), 100},
        };
    }

} // namespace DemoAudio
