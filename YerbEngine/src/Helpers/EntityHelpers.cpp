
#include <Helpers/EntityHelpers.hpp>

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

            float const distanceSquared =
                center.euclideanDistanceSquared(candidateCenter);

            if (distanceSquared >= radiusSquared) {
                continue;
            }
            result.push_back(candidate);
        }

        return result;
    }
} // namespace EntityHelpers
