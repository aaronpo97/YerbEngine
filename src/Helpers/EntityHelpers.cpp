
#include "includes/Helpers/EntityHelpers.hpp"
#include <algorithm>
#include <vector>

namespace EntityHelpers {
  EntityList getEntitiesInRadius(const std::shared_ptr<Entity> &entity,
                                 const EntityList              &candidates,
                                 const float                   &radius) {

    EntityList  result;
    const Vec2 &center        = entity->getCenterPos();
    const float radiusSquared = radius * radius;

    for (const auto &candidate : candidates) {
      if (candidate == entity)
        continue;

      const Vec2 &candidateCenter = candidate->getCenterPos();

      const float deltaX = center.x - candidateCenter.x;
      const float deltaY = center.y - candidateCenter.y;
      const float distanceSquared =
              MathHelpers::pythagorasSquared(deltaX, deltaY);

      if (distanceSquared >= radiusSquared) {
        continue;
      }
      result.push_back(candidate);
    }

    return result;
  }
} // namespace EntityHelpers
