#include <YerbEngine/Helpers/SpawnHelpers.hpp>
#include <YerbEngine/EntityManagement/Entity.hpp>
#include <YerbEngine/Helpers/CollisionHelpers.hpp>
#include <YerbEngine/Helpers/MathHelpers.hpp>

namespace SpawnHelpers {
    Vec2 createRandomPosition(std::mt19937 &randomGenerator,
                              Vec2 const   &windowSize) {
        std::uniform_int_distribution<int> randomXPos(
            0, static_cast<int>(windowSize.x));

        std::uniform_int_distribution<int> randomYPos(
            0, static_cast<int>(windowSize.y));

        return {static_cast<float>(randomXPos(randomGenerator)),
                static_cast<float>(randomYPos(randomGenerator))};
    };

    Vec2 createValidVelocity(std::mt19937 &randomGenerator,
                 int const     attempts) {
      std::uniform_int_distribution<int> randomVel(-1, 1);

      int tries = attempts;
      while (tries-- > 0) {
        int vx = randomVel(randomGenerator);
        int vy = randomVel(randomGenerator);

        if (vx == 0 && vy == 0) {
          continue;
        }

        return Vec2(static_cast<float>(vx), static_cast<float>(vy)).normalize();
      }

      return Vec2(0, 0);
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

        auto const centerA         = player->getCenterPos();
        auto const centerB         = entity->getCenterPos();
        auto const distanceSquared = MathHelpers::pythagorasSquared(
            centerA.x - centerB.x, centerA.y - centerB.y);

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
