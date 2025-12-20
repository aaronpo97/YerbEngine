#pragma once

#include <YerbEngine.hpp>
using namespace YerbEngine;
#include <bitset>
#include <functional>
#include <memory>
#include <random>

namespace ShootDemo::CollisionHelpers::MainScene {
    struct CollisionPair {
        std::shared_ptr<Entity> const &entityA;
        std::shared_ptr<Entity> const &entityB;
    };

    struct GameState {
        EntityManager                  &entityManager;
        std::mt19937                   &randomGenerator;
        int const                       score;
        std::function<void(int)> const &setScore;
        std::function<void()> const     decrementLives;
        AudioSampleBuffer               &audioSampleManager;
        Vec2 const                      windowSize;
    };

    void handleEntityBounds(std::shared_ptr<Entity> const &entity,
                            Vec2 const                    &windowSize);
    void handleEntityEntityCollision(CollisionPair const &collisionPair,
                                     GameState const     &args);

} // namespace ShootDemo::CollisionHelpers::MainScene

namespace ShootDemo::CollisionHelpers::MainScene::Enforce {
    void enforcePlayerBounds(std::shared_ptr<Entity> const &entity,
                             std::bitset<4> const          &collides,
                             Vec2 const                    &window_size);

    void enforceNonPlayerBounds(std::shared_ptr<Entity> const &entity,
                                std::bitset<4> const          &collides);

    void enforceCollisionWithWall(std::shared_ptr<Entity> const &entity,
                                  std::shared_ptr<Entity> const &wall);

    void enforceEntityEntityCollision(std::shared_ptr<Entity> const &entityA,
                                      std::shared_ptr<Entity> const &entityB);

} // namespace ShootDemo::CollisionHelpers::MainScene::Enforce
