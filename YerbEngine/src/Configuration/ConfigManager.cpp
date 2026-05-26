#include <Configuration/ConfigManager.hpp>
#include <fstream>

template <typename JsonReturnType>
JsonReturnType ConfigManager::getJsonValue(json const        &jsonValue,
                                           std::string const &key,
                                           std::string const &context) {
    try {
        return jsonValue.at(key).get<JsonReturnType>();
    } catch (json::exception const &e) {
        throw ConfigurationError("Error parsing " + context + ": " + key +
                                 " - " + e.what());
    }
}

SDL_Color ConfigManager::parseColor(json const        &colorJson,
                                    std::string const &context) {
    auto const red   = getJsonValue<Uint8>(colorJson, "r", context);
    auto const green = getJsonValue<Uint8>(colorJson, "g", context);
    auto const blue  = getJsonValue<Uint8>(colorJson, "b", context);
    auto const alpha = getJsonValue<Uint8>(colorJson, "a", context);

    return {.r = red, .g = green, .b = blue, .a = alpha};
}

ShapeConfig ConfigManager::parseShapeConfig(json const        &shapeJson,
                                            std::string const &context) {
    auto const      height = getJsonValue<float>(shapeJson, "height", context);
    auto const      width  = getJsonValue<float>(shapeJson, "width", context);
    SDL_Color const color  = parseColor(shapeJson["color"], context + ".color");

    return {height, width, color};
}

void ConfigManager::parseGameConfig() {
    auto const &gameConfigJson = m_json["gameConfig"];
    auto const &sizeJson       = gameConfigJson["windowSize"];

    auto const windowWidth =
        getJsonValue<float>(sizeJson, "width", "gameConfig.windowSize");
    auto const windowHeight =
        getJsonValue<float>(sizeJson, "height", "gameConfig.windowSize");
    auto const fontPath =
        getJsonValue<fs::path>(gameConfigJson, "fontPath", "gameConfig");
    auto const windowTitle =
        getJsonValue<std::string>(gameConfigJson, "windowTitle", "gameConfig");
    auto const spawnInterval =
        getJsonValue<Uint64>(gameConfigJson, "spawnInterval", "gameConfig");

    auto const fontSizeSm =
        getJsonValue<int>(gameConfigJson, "fontSizeSm", "gameConfig");
    auto const fontSizeMd =
        getJsonValue<int>(gameConfigJson, "fontSizeMd", "gameConfig");
    auto const fontSizeLg =
        getJsonValue<int>(gameConfigJson, "fontSizeLg", "gameConfig");

    m_gameConfig.windowSize    = Vec2(windowWidth, windowHeight);
    m_gameConfig.windowTitle   = windowTitle;
    m_gameConfig.fontPath      = fontPath;
    m_gameConfig.spawnInterval = spawnInterval;
    m_gameConfig.fontSizeSm    = fontSizeSm;
    m_gameConfig.fontSizeMd    = fontSizeMd;
    m_gameConfig.fontSizeLg    = fontSizeLg;

    if (!fs::exists(m_gameConfig.fontPath)) {
        throw ConfigurationError("Font file not found: " +
                                 m_gameConfig.fontPath.string());
    }
}

void ConfigManager::parseItemConfig() {
    auto const &config = m_json["itemConfig"];

    m_itemConfig.lifespan =
        getJsonValue<Uint64>(config, "lifespan", "itemConfig");
    m_itemConfig.speed = getJsonValue<float>(config, "speed", "itemConfig");
    m_itemConfig.spawnPercentage =
        getJsonValue<Uint8>(config, "spawnPercentage", "itemConfig");
    m_itemConfig.shape = parseShapeConfig(config["shape"], "itemConfig.shape");

    if (m_itemConfig.spawnPercentage > 100) {
        throw ConfigurationError(
            "Item spawn percentage must be between 0 and 100");
    }
}

void ConfigManager::parseEnemyConfig() {
    auto const &config = m_json["enemyConfig"];

    m_enemyConfig.speed = getJsonValue<float>(config, "speed", "enemyConfig");
    m_enemyConfig.lifespan =
        getJsonValue<Uint64>(config, "lifespan", "enemyConfig");
    m_enemyConfig.spawnPercentage =
        getJsonValue<Uint8>(config, "spawnPercentage", "enemyConfig");
    m_enemyConfig.shape =
        parseShapeConfig(config["shape"], "enemyConfig.shape");

    if (m_enemyConfig.spawnPercentage > 100) {
        throw ConfigurationError(
            "Enemy spawn percentage must be between 0 and 100");
    }
}

void ConfigManager::parseSpeedEffectConfig() {
    auto const &config = m_json["speedEffectConfig"];

    m_speedEffectConfig.speed =
        getJsonValue<float>(config, "speed", "speedEffectConfig");
    m_speedEffectConfig.lifespan =
        getJsonValue<Uint64>(config, "lifespan", "speedEffectConfig");
    m_speedEffectConfig.shape =
        parseShapeConfig(config["shape"], "speedEffectConfig.shape");

    m_speedEffectConfig.spawnPercentage = getJsonValue<unsigned int>(
        config, "spawnPercentage", "speedEffectConfig");

    if (m_speedEffectConfig.spawnPercentage > 100) {
        throw ConfigurationError(
            "SpeedBoost spawn percentage must be between 0 and 100");
    }
}

void ConfigManager::parseSlownessEffectConfig() {
    auto const &config = m_json["slownessEffectConfig"];

    m_slownessEffectConfig.speed =
        getJsonValue<float>(config, "speed", "slownessEffectConfig");
    m_slownessEffectConfig.lifespan =
        getJsonValue<Uint64>(config, "lifespan", "slownessEffectConfig");
    m_slownessEffectConfig.spawnPercentage = getJsonValue<unsigned int>(
        config, "spawnPercentage", "slownessEffectConfig");
    m_slownessEffectConfig.shape =
        parseShapeConfig(config["shape"], "slownessEffectConfig.shape");

    if (m_slownessEffectConfig.spawnPercentage > 100) {
        throw ConfigurationError(
            "Slowness spawn percentage must be between 0 and 100");
    }
}

void ConfigManager::parseBulletConfig() {
    auto const &config = m_json["bulletConfig"];

    m_bulletConfig.speed = getJsonValue<float>(config, "speed", "bulletConfig");
    m_bulletConfig.lifespan =
        getJsonValue<Uint64>(config, "lifespan", "bulletConfig");
    m_bulletConfig.shape =
        parseShapeConfig(config["shape"], "bulletConfig.shape");
}

void ConfigManager::parsePlayerConfig() {
    auto const &config = m_json["playerConfig"];

    m_playerConfig.baseSpeed =
        getJsonValue<float>(config, "baseSpeed", "playerConfig");
    m_playerConfig.speedBoostMultiplier =
        getJsonValue<float>(config, "speedBoostMultiplier", "playerConfig");
    m_playerConfig.slownessMultiplier =
        getJsonValue<float>(config, "slownessMultiplier", "playerConfig");
    m_playerConfig.shape =
        parseShapeConfig(config["shape"], "playerConfig.shape");

    if (m_playerConfig.speedBoostMultiplier <= 0 ||
        m_playerConfig.slownessMultiplier <= 0) {
        throw ConfigurationError("Player speed multipliers must be positive");
    }
}

void ConfigManager::parseConfig() {
    try {
        parseGameConfig();
        parsePlayerConfig();
        parseEnemyConfig();
        parseItemConfig();
        parseBulletConfig();
        parseSpeedEffectConfig();
        parseSlownessEffectConfig();
    } catch (json::exception const &e) {
        throw ConfigurationError("JSON parsing error: " +
                                 std::string(e.what()));
    }
}

void ConfigManager::loadConfig() {
    if (!fs::exists(m_configPath)) {
        throw ConfigurationError("Config file not found: " +
                                 m_configPath.string());
    }

    std::ifstream configFile(m_configPath);
    if (!configFile.is_open()) {
        throw ConfigurationError("Failed to open config file: " +
                                 m_configPath.string());
    }

    try {
        configFile >> m_json;
    } catch (json::parse_error const &e) {
        throw ConfigurationError("JSON parse error: " + std::string(e.what()));
    }

    parseConfig();
    configFile.close();
}

ConfigManager::ConfigManager(std::filesystem::path configPath)
    : m_configPath(std::move(configPath)) {
    try {
        loadConfig();
        SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM,
                    "ConfigManager successfully loaded: %s",
                    m_configPath.c_str());
    } catch (ConfigurationError const &e) {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Configuration error: %s",
                     e.what());
        std::cerr << e.what() << std::endl;
        throw;
    }
}

GameConfig const &ConfigManager::getGameConfig() const { return m_gameConfig; }

ItemConfig const &ConfigManager::getItemConfig() const { return m_itemConfig; }

PlayerConfig const &ConfigManager::getPlayerConfig() const {
    return m_playerConfig;
}

EnemyConfig const &ConfigManager::getEnemyConfig() const {
    return m_enemyConfig;
}

BulletConfig const &ConfigManager::getBulletConfig() const {
    return m_bulletConfig;
}

SpeedEffectConfig const &ConfigManager::getSpeedEffectConfig() const {
    return m_speedEffectConfig;
}

SlownessEffectConfig const &ConfigManager::getSlownessEffectConfig() const {
    return m_slownessEffectConfig;
}

void ConfigManager::updatePlayerShape(ShapeConfig const &shape) {
    m_playerConfig.shape = shape;
}

void ConfigManager::updatePlayerSpeed(float const speed) {
    if (speed <= 0) {
        throw ConfigurationError("Player speed must be positive");
    }
    m_playerConfig.baseSpeed = speed;
}

void ConfigManager::updateEnemyShape(ShapeConfig const &shape) {
    m_enemyConfig.shape = shape;
}

void ConfigManager::updateEnemySpeed(float const speed) {
    if (speed <= 0) {
        throw ConfigurationError("Enemy speed must be positive");
    }
    m_enemyConfig.speed = speed;
}

void ConfigManager::updateGameWindowSize(Vec2 const &size) {
    if (size.x() <= 0 || size.y() <= 0) {
        throw ConfigurationError("Window dimensions must be positive");
    }
    m_gameConfig.windowSize = size;
}

void ConfigManager::updateGameWindowTitle(std::string const &title) {
    if (title.empty()) {
        throw ConfigurationError("Window title cannot be empty");
    }
    m_gameConfig.windowTitle = title;
}

void ConfigManager::updateSpeedBoostEffectSpeed(float const speed) {
    if (speed <= 0) {
        throw ConfigurationError("Speed boost effect speed must be positive");
    }
    m_speedEffectConfig.speed = speed;
}

void ConfigManager::updateSlownessEffectSpeed(float const speed) {
    if (speed <= 0) {
        throw ConfigurationError("Slowness effect speed must be positive");
    }
    m_slownessEffectConfig.speed = speed;
}

void ConfigManager::updateBulletSpeed(float const speed) {
    if (speed <= 0) {
        throw ConfigurationError("Bullet speed must be positive");
    }
    m_bulletConfig.speed = speed;
}

void ConfigManager::updateItemSpeed(float const speed) {
    if (speed <= 0) {
        throw ConfigurationError("Item speed must be positive");
    }
    m_itemConfig.speed = speed;
}