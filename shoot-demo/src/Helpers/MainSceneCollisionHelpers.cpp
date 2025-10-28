#include <Helpers/MainSceneCollisionHelpers.hpp>
#include <YerbEngine.hpp>

#include <bitset>

enum Boundaries : Uint8 { TOP, BOTTOM, LEFT, RIGHT };
enum RelativePosition : Uint8 { ABOVE, BELOW, LEFT_OF, RIGHT_OF };

namespace CollisionHelpers::MainScene::Enforce {
    void enforcePlayerBounds(std::shared_ptr<Entity> const &entity,
                             std::bitset<4> const          &collides,
                             Vec2 const                    &window_size) {

        std::shared_ptr<CShape> const &cShape = entity->getComponent<CShape>();
        std::shared_ptr<CTransform> const &cTransform =
            entity->getComponent<CTransform>();

        if (!cShape || !cTransform) {
            SDL_LogError(SDL_LOG_CATEGORY_SYSTEM,
                         "Entity with ID %zu and tag %u lacks a transform or "
                         "shape component.",
                         entity->id(), entity->tag());
        };

        Vec2 &leftCornerPosition = cTransform->topLeftCornerPos;

        if (collides[TOP]) {
            leftCornerPosition.setY(0);
        }
        if (collides[BOTTOM]) {
            leftCornerPosition.setY(window_size.y() -
                                    static_cast<float>(cShape->rect.h));
        }
        if (collides[LEFT]) {
            leftCornerPosition.setX(0);
        }
        if (collides[RIGHT]) {
            leftCornerPosition.setX(window_size.x() -
                                    static_cast<float>(cShape->rect.w));
        }
    }

    void enforceNonPlayerBounds(std::shared_ptr<Entity> const &entity,
                                std::bitset<4> const          &collides) {
        if (entity->tag() == EntityTags::Player) {
            return;
        }

        if (collides.any()) {
            entity->destroy();
        }
    }

    void enforceCollisionWithWall(std::shared_ptr<Entity> const &entity,
                                  std::shared_ptr<Entity> const &wall) {

        auto const &cTransform     = entity->getComponent<CTransform>();
        auto const &cBounceTracker = entity->getComponent<CBounceTracker>();

        Vec2 const &overlap = CollisionHelpers::calculateOverlap(entity, wall);

        bool const mustResolveCollisionVertically   = overlap.x() > overlap.y();
        bool const mustResolveCollisionHorizontally = overlap.x() < overlap.y();
        std::bitset<4> const positionRelativeToWall =
            CollisionHelpers::getPositionRelativeToEntity(entity, wall);

        bool const playerAboveWall   = positionRelativeToWall[ABOVE];
        bool const playerBelowWall   = positionRelativeToWall[BELOW];
        bool const playerLeftOfWall  = positionRelativeToWall[LEFT_OF];
        bool const playerRightOfWall = positionRelativeToWall[RIGHT_OF];

        if (mustResolveCollisionVertically && playerAboveWall) {
            cTransform->topLeftCornerPos.setY(cTransform->topLeftCornerPos.y() -
                                              overlap.y());
            cTransform->velocity.setY(-cTransform->velocity.y());
        }

        if (mustResolveCollisionVertically && playerBelowWall) {
            cTransform->topLeftCornerPos.setY(cTransform->topLeftCornerPos.y() +
                                              overlap.y());
            cTransform->velocity.setY(-cTransform->velocity.y());
        }

        if (mustResolveCollisionHorizontally && playerLeftOfWall) {
            cTransform->topLeftCornerPos.setX(cTransform->topLeftCornerPos.x() -
                                              overlap.x());
            cTransform->velocity.setX(-cTransform->velocity.x());
        }

        if (mustResolveCollisionHorizontally && playerRightOfWall) {
            cTransform->topLeftCornerPos.setX(cTransform->topLeftCornerPos.x() +
                                              overlap.x());
            cTransform->velocity.setX(-cTransform->velocity.x());
        }

        if (cBounceTracker) {
            cBounceTracker->addBounce();
        }
    }

    void enforceEntityEntityCollision(std::shared_ptr<Entity> const &entityA,
                                      std::shared_ptr<Entity> const &entityB) {
        auto const &cTransformA = entityA->getComponent<CTransform>();
        auto const &cTransformB = entityB->getComponent<CTransform>();

        Vec2 const &overlap =
            CollisionHelpers::calculateOverlap(entityA, entityB);

        bool const mustResolveCollisionVertically   = overlap.x() > overlap.y();
        bool const mustResolveCollisionHorizontally = overlap.x() < overlap.y();
        std::bitset<4> const entityARelativePosition =
            CollisionHelpers::getPositionRelativeToEntity(entityA, entityB);
        std::bitset<4> const entityBRelativePosition =
            CollisionHelpers::getPositionRelativeToEntity(entityB, entityA);

        bool const entityAAboveEntityB   = entityARelativePosition[ABOVE];
        bool const entityABelowEntityB   = entityARelativePosition[BELOW];
        bool const entityALeftOfEntityB  = entityARelativePosition[LEFT_OF];
        bool const entityARightOfEntityB = entityARelativePosition[RIGHT_OF];

        bool const entityBAboveEntityA   = entityBRelativePosition[ABOVE];
        bool const entityBBelowEntityA   = entityBRelativePosition[BELOW];
        bool const entityBLeftOfEntityA  = entityBRelativePosition[LEFT_OF];
        bool const entityBRightOfEntityA = entityBRelativePosition[RIGHT_OF];

        if (mustResolveCollisionVertically && entityAAboveEntityB) {
            cTransformA->topLeftCornerPos.setY(
                cTransformA->topLeftCornerPos.y() - overlap.y());
            cTransformA->velocity.setY(-cTransformA->velocity.y());
        }

        if (mustResolveCollisionVertically && entityABelowEntityB) {
            cTransformA->topLeftCornerPos.setY(
                cTransformA->topLeftCornerPos.y() + overlap.y());
            cTransformA->velocity.setY(-cTransformA->velocity.y());
        }

        if (mustResolveCollisionHorizontally && entityALeftOfEntityB) {
            cTransformA->topLeftCornerPos.setX(
                cTransformA->topLeftCornerPos.x() - overlap.x());
            cTransformA->velocity.setX(-cTransformA->velocity.x());
        }

        if (mustResolveCollisionHorizontally && entityARightOfEntityB) {
            cTransformA->topLeftCornerPos.setX(
                cTransformA->topLeftCornerPos.x() + overlap.x());
            cTransformA->velocity.setX(-cTransformA->velocity.x());
        }

        if (mustResolveCollisionVertically && entityBAboveEntityA) {
            cTransformB->topLeftCornerPos.setY(
                cTransformB->topLeftCornerPos.y() - overlap.y());
            cTransformB->velocity.setY(-cTransformB->velocity.y());
        }

        if (mustResolveCollisionVertically && entityBBelowEntityA) {
            cTransformB->topLeftCornerPos.setY(
                cTransformB->topLeftCornerPos.y() + overlap.y());
            cTransformB->velocity.setY(-cTransformB->velocity.y());
        }

        if (mustResolveCollisionHorizontally && entityBLeftOfEntityA) {
            cTransformB->topLeftCornerPos.setX(
                cTransformB->topLeftCornerPos.x() - overlap.x());
            cTransformB->velocity.setX(-cTransformB->velocity.x());
        }

        if (mustResolveCollisionHorizontally && entityBRightOfEntityA) {
            cTransformB->topLeftCornerPos.setX(
                cTransformB->topLeftCornerPos.x() + overlap.x());
            cTransformB->velocity.setX(-cTransformB->velocity.x());
        }
    }

} // namespace CollisionHelpers::MainScene::Enforce

namespace CollisionHelpers::MainScene {
    void handleEntityBounds(std::shared_ptr<Entity> const &entity,
                            Vec2 const                    &windowSize) {
        auto const tag = entity->tag();
        if (tag == EntityTags::SpeedBoost) {
            std::bitset<4> const speedBoostCollides =
                CollisionHelpers::detectOutOfBounds(entity, windowSize);
            Enforce::enforceNonPlayerBounds(entity, speedBoostCollides);
        }

        if (tag == EntityTags::Player) {
            std::bitset<4> const playerCollides =
                CollisionHelpers::detectOutOfBounds(entity, windowSize);
            Enforce::enforcePlayerBounds(entity, playerCollides, windowSize);
        }

        if (tag == EntityTags::Enemy) {
            std::bitset<4> const enemyCollides =
                CollisionHelpers::detectOutOfBounds(entity, windowSize);
            Enforce::enforceNonPlayerBounds(entity, enemyCollides);
        }

        if (tag == EntityTags::SlownessDebuff) {
            std::bitset<4> const slownessCollides =
                CollisionHelpers::detectOutOfBounds(entity, windowSize);
            Enforce::enforceNonPlayerBounds(entity, slownessCollides);
        }

        if (tag == EntityTags::Bullet) {
            std::bitset<4> const bulletCollides =
                CollisionHelpers::detectOutOfBounds(entity, windowSize);
            Enforce::enforceNonPlayerBounds(entity, bulletCollides);
        }
        if (tag == EntityTags::Item) {
            std::bitset<4> const itemCollides =
                CollisionHelpers::detectOutOfBounds(entity, windowSize);
            Enforce::enforceNonPlayerBounds(entity, itemCollides);
        }
    }

    void handleEntityEntityCollision(CollisionPair const &collisionPair,
                                     GameState const     &args) {
        std::shared_ptr<Entity> const &entity      = collisionPair.entityA;
        std::shared_ptr<Entity> const &otherEntity = collisionPair.entityB;

        EntityTags const tag      = entity->tag();
        EntityTags const otherTag = otherEntity->tag();

        constexpr Uint64 minSlownessDuration   = 5000;
        constexpr Uint64 maxSlownessDuration   = 10000;
        constexpr Uint64 minSpeedBoostDuration = 9000;
        constexpr Uint64 maxSpeedBoostDuration = 15000;

        std::uniform_int_distribution<Uint64> randomSlownessDuration(
            minSlownessDuration, maxSlownessDuration);
        std::uniform_int_distribution<Uint64> randomSpeedBoostDuration(
            minSpeedBoostDuration, maxSpeedBoostDuration);

        int const                      m_score           = args.score;
        EntityManager                 &m_entities        = args.entityManager;
        std::mt19937                  &m_randomGenerator = args.randomGenerator;
        std::function<void()> const    decrementLives    = args.decrementLives;
        std::function<void(int)> const setScore          = args.setScore;
        Vec2 const                    &windowSize        = args.windowSize;

        if (entity == otherEntity) {
            return;
        }

        bool const entitiesCollided =
            CollisionHelpers::calculateCollisionBetweenEntities(entity,
                                                                otherEntity);

        if (!entitiesCollided) {
            return;
        }

        if (otherTag == EntityTags::Wall) {
            Enforce::enforceCollisionWithWall(entity, otherEntity);
        }

        if (tag == EntityTags::Enemy && otherTag == EntityTags::Enemy) {
            Enforce::enforceEntityEntityCollision(entity, otherEntity);
        }

        if (tag == EntityTags::Enemy && otherTag == EntityTags::SpeedBoost) {
            Enforce::enforceEntityEntityCollision(entity, otherEntity);
        }

        if (tag == EntityTags::Enemy &&
            otherTag == EntityTags::SlownessDebuff) {
            Enforce::enforceEntityEntityCollision(entity, otherEntity);
        }

        if (tag == EntityTags::Bullet && otherTag == EntityTags::Enemy) {
            AudioSample nextSample = AudioSample::BULLET_HIT_02;
            args.audioSampleManager.queueSample(nextSample,
                                                PriorityLevel::STANDARD);

            auto const &cBounceTracker = entity->getComponent<CBounceTracker>();

            if (!cBounceTracker) {
                entity->destroy();
                return;
            }
            int const bounces = cBounceTracker->getBounces();
            setScore(5 * (bounces + 1) + m_score);
            otherEntity->destroy();
            entity->destroy();
        }

        if (tag == EntityTags::Bullet && otherTag == EntityTags::Wall) {
            args.audioSampleManager.queueSample(AudioSample::BULLET_HIT_01,
                                                PriorityLevel::BACKGROUND);
        }

        if (tag == EntityTags::Bullet &&
            (otherTag == EntityTags::SlownessDebuff ||
             otherTag == EntityTags::SpeedBoost ||
             otherTag == EntityTags::Item)) {
            otherEntity->destroy();
            entity->destroy();

            if (m_score > 15) {
                auto const updatedScore = otherTag == EntityTags::SlownessDebuff
                                              ? m_score + 15
                                              : m_score - 15;
                setScore(updatedScore);
            }
        }

        if (tag == EntityTags::Player && otherTag == EntityTags::Enemy) {
            args.audioSampleManager.queueSample(AudioSample::ENEMY_COLLISION,
                                                PriorityLevel::STANDARD);
            setScore(m_score > 10 ? m_score - 10 : 0);
            otherEntity->destroy();
            decrementLives();

            std::shared_ptr<CTransform> const &cTransform =
                entity->getComponent<CTransform>();
            std::shared_ptr<CEffects> const &cEffects =
                entity->getComponent<CEffects>();
            cTransform->topLeftCornerPos =
                Vec2{windowSize.x() / 2, windowSize.y() / 2};

            constexpr float  REMOVAL_RADIUS = 150.0f;
            EntityList const entitiesToRemove =
                EntityHelpers::getEntitiesInRadius(
                    entity, m_entities.getEntities(EntityTags::Enemy),
                    REMOVAL_RADIUS);

            for (std::shared_ptr<Entity> const &entityToRemove :
                 entitiesToRemove) {
                entityToRemove->destroy();
            }

            cEffects->clearEffects();
        }

        if (tag == EntityTags::Player &&
            otherTag == EntityTags::SlownessDebuff) {
            Uint64 const startTime = SDL_GetTicks64();
            Uint64 const duration  = randomSlownessDuration(m_randomGenerator);

            auto const &cEffects = entity->getComponent<CEffects>();
            cEffects->addEffect({.startTime = startTime,
                                 .duration  = duration,
                                 .type      = EffectTypes::Slowness});

            EntityList        effectsToCheck;
            EntityList const &slownessDebuffs =
                m_entities.getEntities(EntityTags::SlownessDebuff);
            EntityList const &speedBoosts =
                m_entities.getEntities(EntityTags::SpeedBoost);

            effectsToCheck.insert(effectsToCheck.end(), slownessDebuffs.begin(),
                                  slownessDebuffs.end());
            effectsToCheck.insert(effectsToCheck.end(), speedBoosts.begin(),
                                  speedBoosts.end());

            AudioSample const nextSample = AudioSample::SLOWNESS_DEBUFF;
            args.audioSampleManager.queueSample(nextSample,
                                                PriorityLevel::STANDARD);

            constexpr float  REMOVAL_RADIUS = 150.0f;
            EntityList const entitiesToRemove =
                EntityHelpers::getEntitiesInRadius(entity, effectsToCheck,
                                                   REMOVAL_RADIUS);

            for (auto const &entityToRemove : entitiesToRemove) {
                entityToRemove->destroy();
            }

            for (auto const &speedBoost : speedBoosts) {
                speedBoost->destroy();
            }
        }

        if (tag == EntityTags::Player && otherTag == EntityTags::SpeedBoost) {
            Uint64 const startTime = SDL_GetTicks64();
            Uint64 const duration = randomSpeedBoostDuration(m_randomGenerator);
            auto const  &cEffects = entity->getComponent<CEffects>();

            cEffects->addEffect({.startTime = startTime,
                                 .duration  = duration,
                                 .type      = EffectTypes::Speed});

            AudioSample const nextSample = AudioSample::SPEED_BOOST;
            args.audioSampleManager.queueSample(nextSample,
                                                PriorityLevel::STANDARD);

            EntityList const &slownessDebuffs =
                m_entities.getEntities(EntityTags::SlownessDebuff);
            EntityList const &speedBoosts =
                m_entities.getEntities(EntityTags::SpeedBoost);

            constexpr float  REMOVAL_RADIUS = 150.0f;
            EntityList const entitiesToRemove =
                EntityHelpers::getEntitiesInRadius(entity, speedBoosts,
                                                   REMOVAL_RADIUS);

            for (auto const &entityToRemove : entitiesToRemove) {
                entityToRemove->destroy();
            }

            // set the lifespan of the speed boost to 10% of previous value
            for (auto const &speedBoost : speedBoosts) {
                constexpr float MULTIPLIER = 0.1f;
                auto const &cLifespan = speedBoost->getComponent<CLifespan>();
                Uint64     &lifespan  = cLifespan->lifespan;

                lifespan = static_cast<Uint64>(
                    std::round(static_cast<float>(lifespan) * MULTIPLIER));
            }
            for (auto const &slowDebuff : slownessDebuffs) {
                slowDebuff->destroy();
            }
        }

        if (tag == EntityTags::Player && otherTag == EntityTags::Item) {
            args.audioSampleManager.queueSample(AudioSample::ITEM_ACQUIRED,
                                                PriorityLevel::STANDARD);
            setScore(m_score + 90);
            otherEntity->destroy();
        }

        if (tag == EntityTags::Item && otherTag == EntityTags::Enemy) {
            Enforce::enforceEntityEntityCollision(entity, otherEntity);
        }

        if (tag == EntityTags::Item && otherTag == EntityTags::SpeedBoost) {
            Enforce::enforceEntityEntityCollision(entity, otherEntity);
        }

        if (tag == EntityTags::Item && otherTag == EntityTags::SlownessDebuff) {
            Enforce::enforceEntityEntityCollision(entity, otherEntity);
        }
    }

} // namespace CollisionHelpers::MainScene
