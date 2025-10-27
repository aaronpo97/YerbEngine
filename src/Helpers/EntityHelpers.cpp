
#include "includes/Helpers/EntityHelpers.hpp"
#include <algorithm>
#include <vector>

namespace EntityHelpers {
    EntityList getEntitiesInRadius(std::shared_ptr<Entity> const &entity,
                                   EntityList const              &candidates,
                                   float const                   &radius) {

        EntityList  result;
        Vec2 const &center        = entity->getCenterPos();
        float const radiusSquared = radius * radius;

        for (auto const &candidate : candidates) {
            if (candidate == entity)
                continue;

            Vec2 const &candidateCenter = candidate->getCenterPos();

            float const deltaX = center.x - candidateCenter.x;
            float const deltaY = center.y - candidateCenter.y;
            float const distanceSquared =
                MathHelpers::pythagorasSquared(deltaX, deltaY);

            if (distanceSquared >= radiusSquared) {
                continue;
            }
            result.push_back(candidate);
        }

        return result;
    }
} // namespace EntityHelpers
