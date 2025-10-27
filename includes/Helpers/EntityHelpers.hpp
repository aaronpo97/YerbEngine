#pragma once

#include "../EntityManagement/Entity.hpp"
#include "../EntityManagement/EntityManager.hpp"
#include "../Helpers/MathHelpers.hpp"
#include <algorithm>
#include <memory>
#include <vector>

namespace EntityHelpers {
  EntityList getEntitiesInRadius(const std::shared_ptr<Entity> &entity,
                                 const EntityList              &candidates,
                                 const float                   &radius);
} // namespace EntityHelpers
