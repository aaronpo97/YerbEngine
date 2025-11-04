#include <MainScene/MainSceneSpawner.hpp>
#include <YerbEngine.hpp>

MainSceneSpawner::MainSceneSpawner(std::mt19937   &randomGenerator,
                                                                     ConfigAdapter  &config,
                                   TextureManager &textureManager,
                                   EntityManager  &entityManager,
                                   SDL_Renderer   *renderer)
    : m_randomGenerator(randomGenerator),
            m_config(config),
      m_textureManager(textureManager),
      m_entityManager(entityManager),
      m_renderer(renderer) {
    std::cout << "spawner created\n";
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

    auto const cShape =
        std::make_shared<CShape>(m_renderer, playerConfig.shape);
    auto const cTransform =
        std::make_shared<CTransform>(playerPosition, playerVelocity);
    auto const cInput   = std::make_shared<CInput>();
    auto const cEffects = std::make_shared<CEffects>();
    auto const cSprite  = std::make_shared<CSprite>(
        m_textureManager.getTexture(TextureName::PLAYER));

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

    auto const cTransform = std::make_shared<CTransform>(position, velocity);
    auto const cShape = std::make_shared<CShape>(m_renderer, enemyConfig.shape);
    auto const cLifespan = std::make_shared<CLifespan>(enemyConfig.lifespan);

    auto const cSprite = std::make_shared<CSprite>(
        m_textureManager.getTexture(TextureName::ENEMY));

    std::shared_ptr<Entity> const &enemy =
        m_entityManager.addEntity(EntityTags::Enemy);
    enemy->setComponent<CTransform>(cTransform);
    enemy->setComponent<CShape>(cShape);
    enemy->setComponent<CLifespan>(cLifespan);
    enemy->setComponent<CSprite>(cSprite);

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
        enemy->getComponent<CTransform>()->topLeftCornerPos = newPosition;
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

    auto const cTransform = std::make_shared<CTransform>(position, velocity);
    auto const cShape =
        std::make_shared<CShape>(m_renderer, speedEffectConfig.shape);
    auto const cLifespan =
        std::make_shared<CLifespan>(speedEffectConfig.lifespan);

    auto const &speedBoost = m_entityManager.addEntity(EntityTags::SpeedBoost);
    speedBoost->setComponent<CTransform>(cTransform);
    speedBoost->setComponent<CShape>(cShape);
    speedBoost->setComponent<CLifespan>(cLifespan);

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
        speedBoost->getComponent<CTransform>()->topLeftCornerPos = newPosition;
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

    auto const cTransform = std::make_shared<CTransform>(position, velocity);
    auto const cShape =
        std::make_shared<CShape>(m_renderer, slownessEffectConfig.shape);
    auto const cLifespan =
        std::make_shared<CLifespan>(slownessEffectConfig.lifespan);

    std::shared_ptr<Entity> const &slownessEntity =
        m_entityManager.addEntity(EntityTags::SlownessDebuff);

    slownessEntity->setComponent<CTransform>(cTransform);
    slownessEntity->setComponent<CShape>(cShape);
    slownessEntity->setComponent<CLifespan>(cLifespan);

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
        slownessEntity->getComponent<CTransform>()->topLeftCornerPos =
            newPosition;
        isValidSpawn = SpawnHelpers::validateSpawnPosition(
            slownessEntity, player, m_entityManager, windowSize);
        spawnAttempt += 1;
    }

    if (!isValidSpawn) {
        slownessEntity->destroy();
    }

    m_entityManager.update();
}

void MainSceneSpawner::spawnWalls() {
    GameConfig const &gameConfig = m_config.getGameConfig();

    constexpr SDL_Color wallColor  = {.r = 176, .g = 196, .b = 222, .a = 255};
    float const         wallHeight = gameConfig.windowSize.y() * 0.6f;
    float const         wallWidth  = gameConfig.windowSize.x() * 0.025f;

    auto const wallConfig = ShapeConfig(wallHeight, wallWidth, wallColor);

    constexpr size_t WALL_COUNT = 8;

    float const innerWidth  = gameConfig.windowSize.x() * 0.6f;
    float const innerHeight = gameConfig.windowSize.y() * 0.6f;
    float const innerStartX = (gameConfig.windowSize.x() - innerWidth) / 2;
    float const innerStartY = (gameConfig.windowSize.y() - innerHeight) / 2;

    float const outerWidth  = gameConfig.windowSize.x();
    float const outerHeight = gameConfig.windowSize.y();
    float const outerStartX = (gameConfig.windowSize.x() - outerWidth) / 2;
    float const outerStartY = (gameConfig.windowSize.y() - outerHeight) / 2;

    // Gap sizes proportional to respective rectangles
    float const innerGapSize = innerWidth * 0.15f;
    float const outerGapSize = outerWidth * 0.18f;

    for (int i = 0; i < WALL_COUNT; i++) {
        auto const shapeComponent =
            std::make_shared<CShape>(m_renderer, wallConfig);
        auto const transformComponent = std::make_shared<CTransform>();

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

        auto const cSprite = std::make_shared<CSprite>(
            m_textureManager.getTexture(TextureName::WALL));

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
        static_cast<float>(player->getComponent<CShape>()->rect.w) / 2;

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
        std::make_shared<CTransform>(bulletPos, bulletVelocity);
    auto const cLifespan      = std::make_shared<CLifespan>(lifespan);
    auto const cBounceTracker = std::make_shared<CBounceTracker>();
    auto const cShape         = std::make_shared<CShape>(
        m_renderer, ShapeConfig(shape.height, shape.width, shape.color));

    bullet->setComponent<CShape>(cShape);
    bullet->setComponent<CTransform>(cTransform);
    bullet->setComponent<CLifespan>(cLifespan);
    bullet->setComponent<CBounceTracker>(cBounceTracker);

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
    auto const velocity   = Vec2(0, 0);
    auto const cTransform = std::make_shared<CTransform>(position, velocity);
    auto const cShape     = std::make_shared<CShape>(m_renderer, shape);
    auto const cLifespan  = std::make_shared<CLifespan>(lifespan);
    auto const cSprite    = std::make_shared<CSprite>(
        m_textureManager.getTexture(TextureName::COIN));

    auto const &item = m_entityManager.addEntity(EntityTags::Item);
    item->setComponent<CTransform>(cTransform);
    item->setComponent<CShape>(cShape);
    item->setComponent<CLifespan>(cLifespan);
    item->setComponent<CSprite>(cSprite);

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
        item->getComponent<CTransform>()->topLeftCornerPos = newPosition;

        isValidSpawn = SpawnHelpers::validateSpawnPosition(
            item, player, m_entityManager, windowSize);
        spawnAttempt += 1;
    }

    if (!isValidSpawn) {
        item->destroy();
    }

    m_entityManager.update();
}