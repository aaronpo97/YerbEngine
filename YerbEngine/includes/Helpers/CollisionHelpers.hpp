#pragma once

#include <bitset>
#include <functional>
#include <memory>

#include <EntityManagement/Entity.hpp>
#include <Helpers/Vec2.hpp>

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