#include <MainScene/MainSceneSpawner.hpp>
#include <YerbEngine.hpp>
#include <string_view>

namespace {
    constexpr std::string_view PLAYER_TEXTURE_ID      = "player";
    constexpr std::string_view ENEMY_TEXTURE_ID       = "enemy";
    constexpr std::string_view WALL_TEXTURE_ID        = "wall";
    constexpr std::string_view COIN_TEXTURE_ID        = "coin";
    constexpr std::string_view SPEED_BOOST_TEXTURE_ID = "speed_boost";

    void registerDemoTextures(TextureManager &textureManager) {
        auto const registerIfMissing = [&](std::string_view id,
                                           std::string_view path) {
            if (!textureManager.hasTexture(id)) {
                textureManager.registerTexture(id, path);
            }
        };

        registerIfMissing(PLAYER_TEXTURE_ID, "assets/images/player.png");
        registerIfMissing(ENEMY_TEXTURE_ID, "assets/images/enemy.png");
        registerIfMissing(WALL_TEXTURE_ID, "assets/images/wall.png");
        registerIfMissing(COIN_TEXTURE_ID, "assets/images/coin.png");
        registerIfMissing("speed_boost", "assets/images/speedboost.png");
    }
} // namespace

MainSceneSpawner::MainSceneSpawner(std::mt19937      &randomGenerator,
                                   DemoConfigAdapter &config,
                                   TextureManager    &textureManager,
                                   EntityManager     &entityManager,
                                   VideoManager      &videoManager)
    : m_randomGenerator(randomGenerator),
      m_config(config),
      m_videoManager(videoManager),
      m_textureManager(textureManager),
      m_entityManager(entityManager) {
    std::cout << "spawner created\n";
    registerDemoTextures(m_textureManager);
}

std::shared_ptr<Entity> MainSceneSpawner::spawnPlayer() {
    PlayerConfig const &playerConfig = m_config.getPlayerConfig();
    GameConfig const   &gameConfig   = m_config.getGameConfig();

    Vec2 const &windowSize   = gameConfig.windowSize;
    auto const  playerHeight = static_cast<float>(playerConfig.shape.height);
    auto const  playerWidth  = static_cast<float>(playerConfig.shape.width);
    Vec2 const  centerPosition =
        windowSize / 2 - Vec2(playerWidth / 2, playerHeight / 2);

    Vec2 const    &playerPosition = centerPosition;
    constexpr Vec2 playerVelocity{0, 0};

    // auto const cShape = std::make_shared<Components::CShape>(
    //     m_renderer, static_cast<float>(playerConfig.shape.height),
    //     static_cast<float>(playerConfig.shape.width),
    //     playerConfig.shape.color);

    SDL_Rect playerRect{
        .x = 0,
        .y = 0,
        .w = static_cast<int>(playerWidth),
        .h = static_cast<int>(playerHeight),
    };
    auto const cShape = std::make_shared<Components::CShape>(
        playerRect, playerConfig.shape.color);
    auto const cTransform = std::make_shared<Components::CTransform>(
        playerPosition, playerVelocity);
    auto const cInput   = std::make_shared<Components::CInput>();
    auto const cEffects = std::make_shared<Components::CEffects>();
    auto const cSprite =
        std::make_shared<Components::CSprite>(PLAYER_TEXTURE_ID);

    std::shared_ptr<Entity> player =
        m_entityManager.addEntity(EntityTags::Player);
    player->setComponent(cTransform);
    player->setComponent(cShape);
    player->setComponent(cInput);
    player->setComponent(cEffects);
    player->setComponent(cSprite);

    m_entityManager.update();
    return player;
}
void MainSceneSpawner::spawnEnemy(std::shared_ptr<Entity> const &player) {
    constexpr int MAX_SPAWN_ATTEMPTS = 10;

    GameConfig const  &gameConfig  = m_config.getGameConfig();
    EnemyConfig const &enemyConfig = m_config.getEnemyConfig();
    Vec2 const        &windowSize  = gameConfig.windowSize;

    Vec2 const velocity = SpawnHelpers::createValidVelocity(m_randomGenerator);
    Vec2 const position =
        SpawnHelpers::createRandomPosition(m_randomGenerator, windowSize);

    auto const cTransform =
        std::make_shared<Components::CTransform>(position, velocity);
    SDL_Rect enemyRect{
        .x = 0,
        .y = 0,
        .w = static_cast<int>(enemyConfig.shape.width),
        .h = static_cast<int>(enemyConfig.shape.height),
    };

    SDL_Color enemyColor = enemyConfig.shape.color;

    auto const cShape =
        std::make_shared<Components::CShape>(enemyRect, enemyColor);
    auto const cLifespan =
        std::make_shared<Components::CLifespan>(enemyConfig.lifespan);

    auto const cSprite =
        std::make_shared<Components::CSprite>(ENEMY_TEXTURE_ID);

    std::shared_ptr<Entity> const &enemy =
        m_entityManager.addEntity(EntityTags::Enemy);
    enemy->setComponent<Components::CTransform>(cTransform);
    enemy->setComponent<Components::CShape>(cShape);
    enemy->setComponent<Components::CLifespan>(cLifespan);
    enemy->setComponent<Components::CSprite>(cSprite);

    if (!player) {
        SDL_Log("Player missing, destroying enemy");
        enemy->destroy();
        return;
    }

    bool isValidSpawn = SpawnHelpers::validateSpawnPosition(
        enemy, player, m_entityManager, windowSize);
    int spawnAttempt = 1;

    while (!isValidSpawn && spawnAttempt < MAX_SPAWN_ATTEMPTS) {
        auto const newPosition =
            SpawnHelpers::createRandomPosition(m_randomGenerator, windowSize);
        enemy->getComponent<Components::CTransform>()->topLeftCornerPos =
            newPosition;
        isValidSpawn = SpawnHelpers::validateSpawnPosition(
            enemy, player, m_entityManager, windowSize);
        spawnAttempt += 1;
    }

    if (!isValidSpawn) {
        enemy->destroy();
    }

    m_entityManager.update();
}
void MainSceneSpawner::spawnSpeedBoostEntity(
    std::shared_ptr<Entity> const &player) {
    constexpr int MAX_SPAWN_ATTEMPTS = 10;

    GameConfig const        &gameConfig = m_config.getGameConfig();
    SpeedEffectConfig const &speedEffectConfig =
        m_config.getSpeedEffectConfig();
    Vec2 const &windowSize = gameConfig.windowSize;

    Vec2 const velocity = SpawnHelpers::createValidVelocity(m_randomGenerator);
    Vec2 const position =
        SpawnHelpers::createRandomPosition(m_randomGenerator, windowSize);

    auto const cTransform =
        std::make_shared<Components::CTransform>(position, velocity);
    // auto const cShape     = std::make_shared<Components::CShape>(
    //     m_renderer, static_cast<float>(speedEffectConfig.shape.height),
    //     static_cast<float>(speedEffectConfig.shape.width),
    //     speedEffectConfig.shape.color);

    SDL_Rect speedBoostRect{
        .x = 0,
        .y = 0,
        .w = static_cast<int>(speedEffectConfig.shape.width),
        .h = static_cast<int>(speedEffectConfig.shape.height),
    };
    auto const cShape = std::make_shared<Components::CShape>(
        speedBoostRect, speedEffectConfig.shape.color);

    auto const cLifespan =
        std::make_shared<Components::CLifespan>(speedEffectConfig.lifespan);

    auto const &speedBoost = m_entityManager.addEntity(EntityTags::SpeedBoost);
    speedBoost->setComponent<Components::CTransform>(cTransform);
    speedBoost->setComponent<Components::CShape>(cShape);
    speedBoost->setComponent<Components::CLifespan>(cLifespan);
    auto const cSprite =
        std::make_shared<Components::CSprite>(SPEED_BOOST_TEXTURE_ID);
    speedBoost->setComponent<Components::CSprite>(cSprite);

    if (!player) {
        SDL_Log("Player missing, destroying speed boost");
        speedBoost->destroy();
        return;
    }

    bool isValidSpawn = SpawnHelpers::validateSpawnPosition(
        speedBoost, player, m_entityManager, windowSize);
    int spawnAttempt = 1;

    while (!isValidSpawn && spawnAttempt < MAX_SPAWN_ATTEMPTS) {
        auto const newPosition =
            SpawnHelpers::createRandomPosition(m_randomGenerator, windowSize);
        speedBoost->getComponent<Components::CTransform>()->topLeftCornerPos =
            newPosition;
        isValidSpawn = SpawnHelpers::validateSpawnPosition(
            speedBoost, player, m_entityManager, windowSize);
        spawnAttempt += 1;
    }

    if (!isValidSpawn) {
        speedBoost->destroy();
    }

    m_entityManager.update();
}
void MainSceneSpawner::spawnSlownessEntity(
    std::shared_ptr<Entity> const &player) {
    constexpr int MAX_SPAWN_ATTEMPTS = 10;

    GameConfig const &gameConfig = m_config.getGameConfig();

    auto const windowSize = gameConfig.windowSize;

    SlownessEffectConfig const &slownessEffectConfig =
        m_config.getSlownessEffectConfig();

    auto const velocity = SpawnHelpers::createValidVelocity(m_randomGenerator);
    auto const position =
        SpawnHelpers::createRandomPosition(m_randomGenerator, windowSize);

    auto const cTransform =
        std::make_shared<Components::CTransform>(position, velocity);
    // auto const cShape     = std::make_shared<Components::CShape>(
    //     m_renderer, static_cast<float>(slownessEffectConfig.shape.height),
    //     static_cast<float>(slownessEffectConfig.shape.width),
    //     slownessEffectConfig.shape.color);

    auto const slownessRect = SDL_Rect{
        .x = 0,
        .y = 0,
        .w = static_cast<int>(slownessEffectConfig.shape.width),
        .h = static_cast<int>(slownessEffectConfig.shape.height),
    };
    auto const cShape = std::make_shared<Components::CShape>(
        slownessRect, slownessEffectConfig.shape.color);

    auto const cLifespan =
        std::make_shared<Components::CLifespan>(slownessEffectConfig.lifespan);

    std::shared_ptr<Entity> const &slownessEntity =
        m_entityManager.addEntity(EntityTags::SlownessDebuff);

    slownessEntity->setComponent<Components::CTransform>(cTransform);
    slownessEntity->setComponent<Components::CShape>(cShape);
    slownessEntity->setComponent<Components::CLifespan>(cLifespan);

    if (!player) {
        SDL_Log("Player missing destroying slowness debuff");
        slownessEntity->destroy();
        return;
    }

    bool isValidSpawn = SpawnHelpers::validateSpawnPosition(
        slownessEntity, player, m_entityManager, windowSize);
    int spawnAttempt = 1;

    while (!isValidSpawn && spawnAttempt < MAX_SPAWN_ATTEMPTS) {
        auto const newPosition =
            SpawnHelpers::createRandomPosition(m_randomGenerator, windowSize);
        slownessEntity->getComponent<Components::CTransform>()
            ->topLeftCornerPos = newPosition;
        isValidSpawn           = SpawnHelpers::validateSpawnPosition(
            slownessEntity, player, m_entityManager, windowSize);
        spawnAttempt += 1;
    }

    if (!isValidSpawn) {
        slownessEntity->destroy();
    }

    m_entityManager.update();
}

void MainSceneSpawner::spawnWalls() {
    // Get current window size from VideoManager
    Vec2 const windowSize = m_videoManager.getWindowSize();

    constexpr SDL_Color wallColor  = {.r = 176, .g = 196, .b = 222, .a = 255};
    float const         wallHeight = windowSize.y() * 0.6f;
    float const         wallWidth  = windowSize.x() * 0.025f;

    auto const wallConfig = ShapeConfig(wallHeight, wallWidth, wallColor);

    constexpr size_t WALL_COUNT = 8;

    float const innerWidth  = windowSize.x() * 0.6f;
    float const innerHeight = windowSize.y() * 0.6f;
    float const innerStartX = (windowSize.x() - innerWidth) / 2;
    float const innerStartY = (windowSize.y() - innerHeight) / 2;

    float const outerWidth  = windowSize.x();
    float const outerHeight = windowSize.y();
    float const outerStartX = (windowSize.x() - outerWidth) / 2;
    float const outerStartY = (windowSize.y() - outerHeight) / 2;

    // Gap sizes proportional to respective rectangles
    float const innerGapSize = innerWidth * 0.15f;
    float const outerGapSize = outerWidth * 0.18f;

    for (int i = 0; i < WALL_COUNT; i++) {

        SDL_Rect   shapeRect{};
        auto const shapeComponent =
            std::make_shared<Components::CShape>(shapeRect, wallConfig.color);
        auto const transformComponent =
            std::make_shared<Components::CTransform>();

        Vec2 &topLeftCornerPos = transformComponent->topLeftCornerPos;

        bool const isOuterWall       = i >= 4;
        bool const isHorizontal      = (i % 2 == 0);
        bool const isOuterHorizontal = isOuterWall && isHorizontal;
        bool const isOuterVertical   = isOuterWall && !isHorizontal;
        bool const isInnerHorizontal = !isOuterWall && isHorizontal;
        bool const isInnerVertical   = !isOuterWall && !isHorizontal;

        if (isOuterHorizontal) {
            shapeComponent->rect.h = static_cast<int>(wallWidth);
            shapeComponent->rect.w =
                static_cast<int>(outerWidth - (2 * outerGapSize));

            topLeftCornerPos.setX(outerStartX + outerGapSize);
            topLeftCornerPos.setY(
                (i == 4) ? outerStartY : outerStartY + outerHeight - wallWidth);
        }
        if (isOuterVertical) {
            shapeComponent->rect.h =
                static_cast<int>(outerHeight - (2 * outerGapSize));
            shapeComponent->rect.w = static_cast<int>(wallWidth);

            topLeftCornerPos.setX(
                (i == 5) ? outerStartX : outerStartX + outerWidth - wallWidth);
            topLeftCornerPos.setY(outerStartY + outerGapSize);
        }
        if (isInnerHorizontal) {
            shapeComponent->rect.h = static_cast<int>(wallWidth);
            shapeComponent->rect.w =
                static_cast<int>(innerWidth - (2 * innerGapSize));

            topLeftCornerPos.setX(innerStartX + innerGapSize);
            topLeftCornerPos.setY(
                (i == 0) ? innerStartY : innerStartY + innerHeight - wallWidth);
        }
        if (isInnerVertical) {
            shapeComponent->rect.h =
                static_cast<int>(innerHeight - (2 * innerGapSize));
            shapeComponent->rect.w = static_cast<int>(wallWidth);

            topLeftCornerPos.setX(
                (i == 1) ? innerStartX : innerStartX + innerWidth - wallWidth);
            topLeftCornerPos.setY(innerStartY + innerGapSize);
        }

        auto const cSprite =
            std::make_shared<Components::CSprite>(WALL_TEXTURE_ID);

        std::shared_ptr<Entity> const wall =
            m_entityManager.addEntity(EntityTags::Wall);
        wall->setComponent(shapeComponent);
        wall->setComponent(transformComponent);
        wall->setComponent(cSprite);
    }

    m_entityManager.update();
}
void MainSceneSpawner::spawnBullets(std::shared_ptr<Entity> const &player,
                                    Vec2 const &mousePosition) {

    EntityList const walls = m_entityManager.getEntities(EntityTags::Wall);

    auto const &[lifespan, speed, shape] = m_config.getBulletConfig();

    if (!player) {
        SDL_Log("player missing, not creating bullet");
        return;
    }
    Vec2 const &playerCenter = player->getCenterPos();
    float const playerHalfWidth =
        static_cast<float>(player->getComponent<Components::CShape>()->rect.w) /
        2;

    Vec2 direction;
    direction.setX(mousePosition.x() - playerCenter.x());
    direction.setY(mousePosition.y() - playerCenter.y());
    direction.normalize();

    float const                   bulletSpeed    = speed;
    Vec2                          bulletVelocity = direction * bulletSpeed;
    std::shared_ptr<Entity> const bullet =
        m_entityManager.addEntity(EntityTags::Bullet);

    float const bulletHalfWidth  = shape.width / 2;
    float const bulletHalfHeight = shape.height / 2;

    auto const spawnOffset =
        bulletHalfWidth + static_cast<float>(playerHalfWidth);

    Vec2 bulletPos;
    // Set bullet position slightly offset from player center in the direction
    // of travel
    bulletPos.setX(playerCenter.x() + direction.x() * spawnOffset -
                   bulletHalfWidth);
    bulletPos.setY(playerCenter.y() + direction.y() * spawnOffset -
                   bulletHalfHeight);

    auto const cTransform =
        std::make_shared<Components::CTransform>(bulletPos, bulletVelocity);
    auto const cLifespan = std::make_shared<Components::CLifespan>(lifespan);
    auto const cBounceTracker = std::make_shared<Components::CBounceTracker>();
    // auto const cShape = std::make_shared<Components::CShape>(m_renderer,
    // shape.height,
    //                                              shape.width, shape.color);

    SDL_Rect bulletRect{
        .x = 0,
        .y = 0,
        .w = static_cast<int>(shape.width),
        .h = static_cast<int>(shape.height),
    };

    auto const cShape =
        std::make_shared<Components::CShape>(bulletRect, shape.color);

    bullet->setComponent<Components::CShape>(cShape);
    bullet->setComponent<Components::CTransform>(cTransform);
    bullet->setComponent<Components::CLifespan>(cLifespan);
    bullet->setComponent<Components::CBounceTracker>(cBounceTracker);

    for (std::shared_ptr<Entity> const &wall : walls) {
        if (CollisionHelpers::calculateCollisionBetweenEntities(bullet, wall)) {
            bullet->destroy();
            break;
        }
    }

    m_entityManager.update();
}

void MainSceneSpawner::spawnItem(std::shared_ptr<Entity> const &player) {
    constexpr int MAX_SPAWN_ATTEMPTS = 10;

    GameConfig const &gameConfig = m_config.getGameConfig();
    auto const &[spawnPercentage, lifespan, speed, shape] =
        m_config.getItemConfig();
    Vec2 const &windowSize = gameConfig.windowSize;

    auto const position =
        SpawnHelpers::createRandomPosition(m_randomGenerator, windowSize);
    auto const velocity = Vec2(0, 0);
    auto const cTransform =
        std::make_shared<Components::CTransform>(position, velocity);
    // auto const cShape     = std::make_shared<Components::CShape>(m_renderer,
    // shape.height,
    //                                                  shape.width,
    //                                                  shape.color);

    SDL_Rect itemRect{
        .x = 0,
        .y = 0,
        .w = static_cast<int>(shape.width),
        .h = static_cast<int>(shape.height),
    };

    auto const cShape =
        std::make_shared<Components::CShape>(itemRect, shape.color);
    auto const cLifespan = std::make_shared<Components::CLifespan>(lifespan);
    auto const cSprite = std::make_shared<Components::CSprite>(COIN_TEXTURE_ID);

    auto const &item = m_entityManager.addEntity(EntityTags::Item);
    item->setComponent<Components::CTransform>(cTransform);
    item->setComponent<Components::CShape>(cShape);
    item->setComponent<Components::CLifespan>(cLifespan);
    item->setComponent<Components::CSprite>(cSprite);

    if (!player) {
        SDL_Log("Player missing, destroying item entity");
        item->destroy();
        return;
    }

    bool isValidSpawn = SpawnHelpers::validateSpawnPosition(
        item, player, m_entityManager, windowSize);
    int spawnAttempt = 1;

    while (!isValidSpawn && spawnAttempt < MAX_SPAWN_ATTEMPTS) {
        auto const newPosition =
            SpawnHelpers::createRandomPosition(m_randomGenerator, windowSize);
        item->getComponent<Components::CTransform>()->topLeftCornerPos =
            newPosition;

        isValidSpawn = SpawnHelpers::validateSpawnPosition(
            item, player, m_entityManager, windowSize);
        spawnAttempt += 1;
    }

    if (!isValidSpawn) {
        item->destroy();
    }

    m_entityManager.update();
}
