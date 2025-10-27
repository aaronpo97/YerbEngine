#pragma once

#include <bitset>
#include <functional>
#include <iostream>
#include <memory>
#include <random>

#include "../AssetManagement/AudioSampleQueue.hpp"
#include "../EntityManagement/Entity.hpp"
#include "../EntityManagement/EntityManager.hpp"
#include "../Helpers/Vec2.hpp"
#include "../SystemManagement/AudioManager.hpp"

namespace CollisionHelpers {

    std::bitset<4> detectOutOfBounds(std::shared_ptr<Entity> const &entity,
                                     Vec2 const &window_size);

    bool
    calculateCollisionBetweenEntities(std::shared_ptr<Entity> const &entityA,
                                      std::shared_ptr<Entity> const &entityB);

    Vec2 calculateOverlap(std::shared_ptr<Entity> const &entityA,
                          std::shared_ptr<Entity> const &entityB);

    std::bitset<4>
    getPositionRelativeToEntity(std::shared_ptr<Entity> const &entityA,
                                std::shared_ptr<Entity> const &entityB);

} // namespace CollisionHelpers

namespace CollisionHelpers::MainScene {
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
        AudioSampleQueue               &audioSampleManager;
        Vec2 const                      windowSize;
    };

    void handleEntityBounds(std::shared_ptr<Entity> const &entity,
                            Vec2 const                    &windowSize);
    void handleEntityEntityCollision(CollisionPair const &collisionPair,
                                     GameState const     &args);

} // namespace CollisionHelpers::MainScene

namespace CollisionHelpers::MainScene::Enforce {
    void enforcePlayerBounds(std::shared_ptr<Entity> const &entity,
                             std::bitset<4> const          &collides,
                             Vec2 const                    &window_size);

    void enforceNonPlayerBounds(std::shared_ptr<Entity> const &entity,
                                std::bitset<4> const          &collides);

    void enforceCollisionWithWall(std::shared_ptr<Entity> const &entity,
                                  std::shared_ptr<Entity> const &wall);

    void enforceEntityEntityCollision(std::shared_ptr<Entity> const &entityA,
                                      std::shared_ptr<Entity> const &entityB);

} // namespace CollisionHelpers::MainScene::Enforce