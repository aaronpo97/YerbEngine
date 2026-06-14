#pragma once

#include "./ConfigDeprecated.hpp"
#include <filesystem>
#include <nlohmann/json.hpp>
#include <stdexcept>

using json   = nlohmann::json;
namespace fs = std::filesystem;
namespace YerbEngine {
    class ConfigurationError : public std::runtime_error {
      public:
        explicit ConfigurationError(std::string const &message)
            : std::runtime_error(message) {}
    };

    class ConfigManagerDeprecated {
        GameConfig            m_gameConfig;
        PlayerConfig          m_playerConfig;
        EnemyConfig           m_enemyConfig;
        BulletConfig          m_bulletConfig;
        ItemConfig            m_itemConfig;
        SpeedEffectConfig     m_speedEffectConfig;
        SlownessEffectConfig  m_slownessEffectConfig;
        json                  m_json;
        std::filesystem::path m_configPath;

        template <typename JsonReturnType>
        static JsonReturnType getJsonValue(json const        &jsonValue,
                                           std::string const &key,
                                           std::string const &context);

        static SDL_Color   parseColor(json const        &colorJson,
                                      std::string const &context);
        static ShapeConfig parseShapeConfig(json const        &shapeJson,
                                            std::string const &context);
        void               parseGameConfig();
        void               parseItemConfig();
        void               parsePlayerConfig();
        void               parseEnemyConfig();
        void               parseSpeedEffectConfig();
        void               parseSlownessEffectConfig();
        void               parseBulletConfig();
        void               parseConfig();
        void               loadConfig();

      public:
        explicit ConfigManagerDeprecated(
            std::filesystem::path configPath = "assets/config.json");

        GameConfig const           &getGameConfig() const;
        ItemConfig const           &getItemConfig() const;
        PlayerConfig const         &getPlayerConfig() const;
        EnemyConfig const          &getEnemyConfig() const;
        BulletConfig const         &getBulletConfig() const;
        SpeedEffectConfig const    &getSpeedEffectConfig() const;
        SlownessEffectConfig const &getSlownessEffectConfig() const;

        void updatePlayerShape(ShapeConfig const &shape);
        void updatePlayerSpeed(float speed);
        void updateEnemyShape(ShapeConfig const &shape);
        void updateEnemySpeed(float speed);
        void updateGameWindowSize(Vec2 const &size);
        void updateGameWindowTitle(std::string const &title);
        void updateSpeedBoostEffectSpeed(float speed);
        void updateSlownessEffectSpeed(float speed);
        void updateBulletSpeed(float speed);
        void updateItemSpeed(float speed);
    };
} // namespace YerbEngine