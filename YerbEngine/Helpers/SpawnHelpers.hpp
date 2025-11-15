#pragma once

#include <YerbEngine/AssetManagement/TextureManager.hpp>
#include <YerbEngine/Configuration/ConfigManager.hpp>
#include <YerbEngine/EntityManagement/Entity.hpp>
#include <YerbEngine/EntityManagement/EntityManager.hpp>
#include <YerbEngine/Helpers/Vec2.hpp>

#include <SDL.h>
#include <iostream>
#include <memory>
#include <random>

namespace SpawnHelpers {
    Vec2 createRandomPosition(std::mt19937 &randomGenerator,
                              Vec2 const   &windowSize);
    Vec2 createValidVelocity(std::mt19937 &randomGenerator, int attempts = 5);
    bool validateSpawnPosition(std::shared_ptr<Entity> const &entity,
                               std::shared_ptr<Entity> const &player,
                               EntityManager                 &entityManager,
                               Vec2 const                    &windowSize);
} // namespace SpawnHelpers