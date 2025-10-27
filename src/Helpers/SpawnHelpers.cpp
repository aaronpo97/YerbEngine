#include "includes/Helpers/SpawnHelpers.hpp"
#include "includes/EntityManagement/Entity.hpp"
#include "includes/Helpers/CollisionHelpers.hpp"
#include "includes/Helpers/MathHelpers.hpp"

namespace SpawnHelpers {
    Vec2 createRandomPosition(std::mt19937 &randomGenerator,
                              Vec2 const   &windowSize) {
        std::uniform_int_distribution<int> randomXPos(
            0, static_cast<int>(windowSize.x));
        std::uniform_int_distribution<int> randomYPos(
            0, static_cast<int>(windowSize.y));
        int const xPos = randomXPos(randomGenerator);
        int const yPos = randomYPos(randomGenerator);
        return {static_cast<float>(xPos), static_cast<float>(yPos)};
    };

    Vec2 createValidVelocity(std::mt19937 &randomGenerator,
                             int const     attempts) {
        std::uniform_int_distribution<int> randomVel(-1, 1);

        /*
         * 8/9 chance of generating a valid velocity on the first attempt.
         */
        auto const velocity =
            Vec2(static_cast<float>(randomVel(randomGenerator)),
                 static_cast<float>(randomVel(randomGenerator)))
                .normalize();

        /*
         *  Has a (1/9)^n chance of falling back to the default vector after n
         * attempts.
         */
        if (attempts <= 0) {
            return Vec2(1, 0).normalize();
        }

        return (velocity == Vec2(0, 0))
                   ? createValidVelocity(randomGenerator, attempts - 1)
                   : velocity;
    };

    bool validateSpawnPosition(std::shared_ptr<Entity> const &entity,
                               std::shared_ptr<Entity> const &player,
                               EntityManager                 &entityManager,
                               Vec2 const                    &windowSize) {
        constexpr int MIN_DISTANCE_TO_PLAYER = 40;

        bool const touchesBoundary =
            CollisionHelpers::detectOutOfBounds(entity, windowSize).any();

        if (touchesBoundary) {
            return false;
        }

        auto calculateDistanceSquared =
            [](std::shared_ptr<Entity> const &entityA,
               std::shared_ptr<Entity> const &entityB) -> float {
            auto const centerA = entityA->getCenterPos();
            auto const centerB = entityB->getCenterPos();
            return MathHelpers::pythagorasSquared(centerA.x - centerB.x,
                                                  centerA.y - centerB.y);
        };

        float const distanceSquared = calculateDistanceSquared(player, entity);
        if (distanceSquared < MIN_DISTANCE_TO_PLAYER * MIN_DISTANCE_TO_PLAYER) {
            return false;
        }

        auto collisionCheck =
            [&](std::shared_ptr<Entity> const &entityToCheck) -> bool {
            return CollisionHelpers::calculateCollisionBetweenEntities(
                entity, entityToCheck);
        };

        bool const isCollidingWithOtherEntities =
            std::ranges::any_of(entityManager.getEntities(), collisionCheck);

        if (isCollidingWithOtherEntities) {
            return false;
        }
        return true;
    };
} // namespace SpawnHelpers
