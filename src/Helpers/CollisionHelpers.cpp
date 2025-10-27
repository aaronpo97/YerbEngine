#include "includes/Helpers/CollisionHelpers.hpp"
#include "includes/EntityManagement/Entity.hpp"
#include "includes/GameScenes/MainScene/MainScene.hpp"
#include "includes/Helpers/EntityHelpers.hpp"

#include <bitset>

enum Boundaries : Uint8 { TOP, BOTTOM, LEFT, RIGHT };
enum RelativePosition : Uint8 { ABOVE, BELOW, LEFT_OF, RIGHT_OF };

namespace CollisionHelpers {
    std::bitset<4> detectOutOfBounds(std::shared_ptr<Entity> const &entity,
                                     Vec2 const &window_size) {

        std::shared_ptr<CTransform> const &cTransform =
            entity->getComponent<CTransform>();
        std::shared_ptr<CShape> const &cShape = entity->getComponent<CShape>();

        if (!cTransform || !cShape) {
            SDL_LogError(SDL_LOG_CATEGORY_SYSTEM,
                         "Entity with ID %zu and tag %u lacks a transform or "
                         "shape component.",
                         entity->id(), entity->tag());

            return {};
        }

        Vec2 const &topLeftCorner = cTransform->topLeftCornerPos;
        auto const  rectHeight    = static_cast<float>(cShape->rect.h);
        auto const  rectWidth     = static_cast<float>(cShape->rect.w);

        bool const collidesWithTop = topLeftCorner.y <= 0;
        bool const collidesWithBottom =
            topLeftCorner.y + rectHeight >= window_size.y;
        bool const collidesWithLeft = topLeftCorner.x <= 0;
        bool const collidesWithRight =
            topLeftCorner.x + rectWidth >= window_size.x;

        std::bitset<4> collidesWithBoundary;
        collidesWithBoundary[TOP]    = collidesWithTop;
        collidesWithBoundary[BOTTOM] = collidesWithBottom;
        collidesWithBoundary[LEFT]   = collidesWithLeft;
        collidesWithBoundary[RIGHT]  = collidesWithRight;

        return collidesWithBoundary;
    }

    Vec2 calculateOverlap(std::shared_ptr<Entity> const &entityA,
                          std::shared_ptr<Entity> const &entityB) {

        auto const &cShapeA = entityA->getComponent<CShape>();
        auto const &cShapeB = entityB->getComponent<CShape>();

        if (!cShapeA) {
            SDL_LogError(SDL_LOG_CATEGORY_SYSTEM,
                         "Entity with ID %zu and tag %u lacks a collision "
                         "component.",
                         entityA->id(), entityA->tag());
            return {0, 0};
        }

        if (!cShapeB) {
            SDL_LogError(SDL_LOG_CATEGORY_SYSTEM,
                         "Entity with ID %zu and tag %u lacks a collision "
                         "component.",
                         entityB->id(), entityB->tag());
            return {0, 0};
        }

        float const halfWidthA  = static_cast<float>(cShapeA->rect.w) / 2.0f;
        float const halfHeightA = static_cast<float>(cShapeA->rect.h) / 2.0f;

        float const halfWidthB  = static_cast<float>(cShapeB->rect.w) / 2.0f;
        float const halfHeightB = static_cast<float>(cShapeB->rect.h) / 2.0f;

        auto const halfSizeA = Vec2(halfWidthA, halfHeightA);
        auto const halfSizeB = Vec2(halfWidthB, halfHeightB);

        Vec2 const &centerA = entityA->getCenterPos();
        Vec2 const &centerB = entityB->getCenterPos();

        auto const delta = Vec2(std::abs(centerA.x - centerB.x),
                                std::abs(centerA.y - centerB.y));

        Vec2 const overlap = {
            halfSizeA.x + halfSizeB.x - delta.x,
            halfSizeA.y + halfSizeB.y - delta.y,
        };

        return overlap;
    }

    bool
    calculateCollisionBetweenEntities(std::shared_ptr<Entity> const &entityA,
                                      std::shared_ptr<Entity> const &entityB) {
        Vec2 const overlap           = calculateOverlap(entityA, entityB);
        bool const collisionDetected = overlap.x > 0 && overlap.y > 0;
        return collisionDetected;
    }

    std::bitset<4>
    getPositionRelativeToEntity(std::shared_ptr<Entity> const &entityA,
                                std::shared_ptr<Entity> const &entityB) {
        Vec2 const &centerA = entityA->getCenterPos();
        Vec2 const &centerB = entityB->getCenterPos();

        std::bitset<4> relativePosition;
        relativePosition[ABOVE]    = centerA.y < centerB.y;
        relativePosition[BELOW]    = centerA.y > centerB.y;
        relativePosition[LEFT_OF]  = centerA.x < centerB.x;
        relativePosition[RIGHT_OF] = centerA.x > centerB.x;

        return relativePosition;
    }

} // namespace CollisionHelpers

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
            leftCornerPosition.y = 0;
        }
        if (collides[BOTTOM]) {
            leftCornerPosition.y =
                window_size.y - static_cast<float>(cShape->rect.h);
        }
        if (collides[LEFT]) {
            leftCornerPosition.x = 0;
        }
        if (collides[RIGHT]) {
            leftCornerPosition.x =
                window_size.x - static_cast<float>(cShape->rect.w);
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

        Vec2 const &overlap = calculateOverlap(entity, wall);

        bool const mustResolveCollisionVertically   = overlap.x > overlap.y;
        bool const mustResolveCollisionHorizontally = overlap.x < overlap.y;
        std::bitset<4> const positionRelativeToWall =
            getPositionRelativeToEntity(entity, wall);

        bool const playerAboveWall   = positionRelativeToWall[ABOVE];
        bool const playerBelowWall   = positionRelativeToWall[BELOW];
        bool const playerLeftOfWall  = positionRelativeToWall[LEFT_OF];
        bool const playerRightOfWall = positionRelativeToWall[RIGHT_OF];

        if (mustResolveCollisionVertically && playerAboveWall) {
            cTransform->topLeftCornerPos.y -= overlap.y;
            cTransform->velocity.y = -cTransform->velocity.y;
        }

        if (mustResolveCollisionVertically && playerBelowWall) {
            cTransform->topLeftCornerPos.y += overlap.y;
            cTransform->velocity.y = -cTransform->velocity.y;
        }

        if (mustResolveCollisionHorizontally && playerLeftOfWall) {
            cTransform->topLeftCornerPos.x -= overlap.x;
            cTransform->velocity.x = -cTransform->velocity.x;
        }

        if (mustResolveCollisionHorizontally && playerRightOfWall) {
            cTransform->topLeftCornerPos.x += overlap.x;
            cTransform->velocity.x = -cTransform->velocity.x;
        }

        if (cBounceTracker) {
            cBounceTracker->addBounce();
        }
    }

    void enforceEntityEntityCollision(std::shared_ptr<Entity> const &entityA,
                                      std::shared_ptr<Entity> const &entityB) {
        auto const &cTransformA = entityA->getComponent<CTransform>();
        auto const &cTransformB = entityB->getComponent<CTransform>();

        Vec2 const &overlap = calculateOverlap(entityA, entityB);

        bool const mustResolveCollisionVertically   = overlap.x > overlap.y;
        bool const mustResolveCollisionHorizontally = overlap.x < overlap.y;
        std::bitset<4> const entityARelativePosition =
            getPositionRelativeToEntity(entityA, entityB);
        std::bitset<4> const entityBRelativePosition =
            getPositionRelativeToEntity(entityB, entityA);

        bool const entityAAboveEntityB   = entityARelativePosition[ABOVE];
        bool const entityABelowEntityB   = entityARelativePosition[BELOW];
        bool const entityALeftOfEntityB  = entityARelativePosition[LEFT_OF];
        bool const entityARightOfEntityB = entityARelativePosition[RIGHT_OF];

        bool const entityBAboveEntityA   = entityBRelativePosition[ABOVE];
        bool const entityBBelowEntityA   = entityBRelativePosition[BELOW];
        bool const entityBLeftOfEntityA  = entityBRelativePosition[LEFT_OF];
        bool const entityBRightOfEntityA = entityBRelativePosition[RIGHT_OF];

        if (mustResolveCollisionVertically && entityAAboveEntityB) {
            cTransformA->topLeftCornerPos.y -= overlap.y;
            cTransformA->velocity.y = -cTransformA->velocity.y;
        }

        if (mustResolveCollisionVertically && entityABelowEntityB) {
            cTransformA->topLeftCornerPos.y += overlap.y;
            cTransformA->velocity.y = -cTransformA->velocity.y;
        }

        if (mustResolveCollisionHorizontally && entityALeftOfEntityB) {
            cTransformA->topLeftCornerPos.x -= overlap.x;
            cTransformA->velocity.x = -cTransformA->velocity.x;
        }

        if (mustResolveCollisionHorizontally && entityARightOfEntityB) {
            cTransformA->topLeftCornerPos.x += overlap.x;
            cTransformA->velocity.x = -cTransformA->velocity.x;
        }

        if (mustResolveCollisionVertically && entityBAboveEntityA) {
            cTransformB->topLeftCornerPos.y -= overlap.y;
            cTransformB->velocity.y = -cTransformB->velocity.y;
        }

        if (mustResolveCollisionVertically && entityBBelowEntityA) {
            cTransformB->topLeftCornerPos.y += overlap.y;
            cTransformB->velocity.y = -cTransformB->velocity.y;
        }

        if (mustResolveCollisionHorizontally && entityBLeftOfEntityA) {
            cTransformB->topLeftCornerPos.x -= overlap.x;
            cTransformB->velocity.x = -cTransformB->velocity.x;
        }

        if (mustResolveCollisionHorizontally && entityBRightOfEntityA) {
            cTransformB->topLeftCornerPos.x += overlap.x;
            cTransformB->velocity.x = -cTransformB->velocity.x;
        }
    }

} // namespace CollisionHelpers::MainScene::Enforce

namespace CollisionHelpers::MainScene {
    void handleEntityBounds(std::shared_ptr<Entity> const &entity,
                            Vec2 const                    &windowSize) {
        auto const tag = entity->tag();
        if (tag == EntityTags::SpeedBoost) {
            std::bitset<4> const speedBoostCollides =
                detectOutOfBounds(entity, windowSize);
            Enforce::enforceNonPlayerBounds(entity, speedBoostCollides);
        }

        if (tag == EntityTags::Player) {
            std::bitset<4> const playerCollides =
                detectOutOfBounds(entity, windowSize);
            Enforce::enforcePlayerBounds(entity, playerCollides, windowSize);
        }

        if (tag == EntityTags::Enemy) {
            std::bitset<4> const enemyCollides =
                detectOutOfBounds(entity, windowSize);
            Enforce::enforceNonPlayerBounds(entity, enemyCollides);
        }

        if (tag == EntityTags::SlownessDebuff) {
            std::bitset<4> const slownessCollides =
                detectOutOfBounds(entity, windowSize);
            Enforce::enforceNonPlayerBounds(entity, slownessCollides);
        }

        if (tag == EntityTags::Bullet) {
            std::bitset<4> const bulletCollides =
                detectOutOfBounds(entity, windowSize);
            Enforce::enforceNonPlayerBounds(entity, bulletCollides);
        }
        if (tag == EntityTags::Item) {
            std::bitset<4> const itemCollides =
                detectOutOfBounds(entity, windowSize);
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
                                                AudioSamplePriority::STANDARD);

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
            args.audioSampleManager.queueSample(
                AudioSample::BULLET_HIT_01, AudioSamplePriority::BACKGROUND);
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
                                                AudioSamplePriority::STANDARD);
            setScore(m_score > 10 ? m_score - 10 : 0);
            otherEntity->destroy();
            decrementLives();

            std::shared_ptr<CTransform> const &cTransform =
                entity->getComponent<CTransform>();
            std::shared_ptr<CEffects> const &cEffects =
                entity->getComponent<CEffects>();
            cTransform->topLeftCornerPos = {windowSize.x / 2, windowSize.y / 2};

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
                                                AudioSamplePriority::STANDARD);

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
                                                AudioSamplePriority::STANDARD);

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
                                                AudioSamplePriority::STANDARD);
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