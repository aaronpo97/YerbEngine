#pragma once

#include <EntityManagement/Entity.hpp>
#include <EntityManagement/EntityManager.hpp>
#include <Helpers/Vec2.hpp>

#include <memory>
#include <random>

namespace YerbEngine {

    namespace SpawnHelpers {
    Vec2 createRandomPosition(std::mt19937 &randomGenerator,
                              Vec2 const   &windowSize);
    Vec2 createValidVelocity(std::mt19937 &randomGenerator, int attempts = 5);
    bool validateSpawnPosition(std::shared_ptr<Entity> const &entity,
                               std::shared_ptr<Entity> const &player,
                               EntityManager                 &entityManager,
                               Vec2 const                    &windowSize);
    } // namespace SpawnHelpers

} // namespace YerbEngine