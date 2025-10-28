#pragma once

#include <EntityManagement/Entity.hpp>
#include <EntityManagement/EntityManager.hpp>
#include <memory>

namespace EntityHelpers {
    EntityList getEntitiesInRadius(std::shared_ptr<Entity> const &entity,
                                   EntityList const              &candidates,
                                   float const                   &radius);
} // namespace EntityHelpers
